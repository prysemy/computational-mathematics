#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Построение графиков для задачи фильтрации
"""

import numpy as np
import matplotlib.pyplot as plt
import glob
import os

# ============================================================================
# НАСТРОЙКИ
# ============================================================================

OUTPUT_DIR = "output"
PLOTS_DIR = "plots"
P_ATM = 101325.0

os.makedirs(PLOTS_DIR, exist_ok=True)


# ============================================================================
# ЗАГРУЗКА ДАННЫХ
# ============================================================================

def load_profile(filepath):
    """Загрузка профиля из файла"""
    x, p_pa, p_atm = [], [], []
    t_days = None

    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if line.startswith('#'):
                if 's =' in line and 'days' in line:
                    try:
                        t_days = float(line.split('s =')[1].split('days')[0].strip())
                    except:
                        pass
            else:
                parts = line.split()
                if len(parts) >= 3:
                    x.append(float(parts[0]))
                    p_pa.append(float(parts[1]))
                    p_atm.append(float(parts[2]))

    return np.array(x), np.array(p_atm), t_days


# ============================================================================
# ПОСТРОЕНИЕ ГРАФИКОВ
# ============================================================================

def main():
    # Загрузка всех профилей
    files = sorted(glob.glob(os.path.join(OUTPUT_DIR, "profile_*.dat")))

    if not files:
        print("Нет файлов данных!")
        return

    profiles = []
    for fname in files:
        x, p_atm, t_days = load_profile(fname)
        if t_days is not None and len(x) > 0:
            profiles.append({'time': t_days, 'x': x, 'p': p_atm})

    profiles.sort(key=lambda p: p['time'])
    print(f"Загружено {len(profiles)} профилей")

    # ========================================================================
    # ГРАФИК 1: 4 подграфика как в образце (в Паскалях)
    # ========================================================================

    sample_times = [0.1, 0.25, 0.5, 1.0]
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    axes = axes.flatten()

    for idx, t_target in enumerate(sample_times):
        # Найти ближайший профиль
        prof = min(profiles, key=lambda p: abs(p['time'] - t_target))

        # Перевод в Па
        p_pa = prof['p'] * P_ATM

        axes[idx].plot(prof['x'], p_pa, 'b-', linewidth=2)
        axes[idx].set_xlabel('x, м', fontsize=10)
        axes[idx].set_ylabel('p, Па', fontsize=10)
        axes[idx].set_title(f'P for t = {prof["time"]:.2f} days', fontsize=11)
        axes[idx].grid(True, alpha=0.3)
        axes[idx].set_xlim(0, 500)
        axes[idx].set_ylim(5e6, 1.55e7)
        axes[idx].ticklabel_format(style='sci', axis='y', scilimits=(0, 0))

    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, 'sample_profiles.png'), dpi=300)
    plt.close()
    print("✓ Сохранен: sample_profiles.png")

    # ========================================================================
    # ГРАФИК 2: Эволюция давления (в атмосферах)
    # ========================================================================

    plt.figure(figsize=(12, 8))

    colors = plt.cm.viridis(np.linspace(0, 0.9, len(profiles)))

    for idx, prof in enumerate(profiles):
        t = prof['time']
        # Рисуем ключевые моменты
        if t == 0 or t >= 9.9 or t in [0.1, 0.25, 0.5, 1.0, 2.0, 5.0]:
            label = f't = {t:.2f} дн.' if t < 1 else f't = {t:.1f} дн.'
            plt.plot(prof['x'], prof['p'], '-', linewidth=2,
                     label=label, color=colors[idx], alpha=0.8)

    # Граничные условия
    plt.axhline(y=150, color='blue', linestyle=':', alpha=0.5,
                linewidth=1.5, label='p_inj = 150 атм')
    plt.axhline(y=50, color='red', linestyle=':', alpha=0.5,
                linewidth=1.5, label='p_prod = 50 атм')

    plt.xlabel('Расстояние x, м', fontsize=12)
    plt.ylabel('Давление p, атм', fontsize=12)
    plt.title('Эволюция распределения давления в пласте',
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(ncol=2, fontsize=10)
    plt.xlim(0, 500)
    plt.ylim(45, 155)

    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, 'pressure_evolution.png'), dpi=300)
    plt.close()
    print("✓ Сохранен: pressure_evolution.png")

    print(f"\n✅ Все графики сохранены в папке {PLOTS_DIR}/")


if __name__ == "__main__":
    main()