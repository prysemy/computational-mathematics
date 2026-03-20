import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter
import glob
import os
from pathlib import Path

# Параметры
X_LEFT = -0.5
X_RIGHT = 0.5
T_MAX = 0.02

# Создаем папку для графиков
plots_dir = Path("gas_plots")
plots_dir.mkdir(exist_ok=True)


def load_data():
    """Загружает все CSV файлы из папки gas_data"""
    data_files = glob.glob("/home/emily/Repos/computational-mathematics/Ryman/gas_data/*.csv")
    data = {}

    if not data_files:
        print("  No files found in gas_data/")
        return data

    for f in data_files:
        try:
            df = pd.read_csv(f)
            if df.empty:
                print(f"  Warning: {os.path.basename(f)} is empty")
                continue

            # Проверяем на NaN и Inf
            if df.isnull().values.any() or np.isinf(df.select_dtypes(include=[np.number]).values).any():
                print(f"  Warning: {os.path.basename(f)} contains NaN or Inf - skipping")
                continue

            # Проверяем, есть ли колонка 't'
            if 't' in df.columns:
                t = df['t'].iloc[0]
            else:
                # Если нет колонки t, берем время из имени файла
                filename = os.path.basename(f)
                if filename.startswith('t_'):
                    t_str = filename.replace('t_', '').replace('.csv', '')
                    try:
                        t = float(t_str)
                    except:
                        t = 0.0
                else:
                    t = 0.0
                # Добавляем колонку t в DataFrame
                df['t'] = t

            data[t] = df
            print(f"  Loaded: t={t:.6f}, {os.path.basename(f)}")

        except Exception as e:
            print(f"  Error loading {os.path.basename(f)}: {e}")

    return data


def safe_limits(values, factor_low=0.95, factor_high=1.05):
    """Безопасно вычисляет пределы для графика, отбрасывая NaN/Inf"""
    # Убираем NaN и Inf
    clean_values = values[np.isfinite(values)]
    if len(clean_values) == 0:
        return -1, 1  # значения по умолчанию

    vmin = clean_values.min()
    vmax = clean_values.max()

    # Если min и max равны, расширяем диапазон
    if abs(vmax - vmin) < 1e-10:
        if abs(vmin) < 1e-10:
            return -0.1, 0.1
        else:
            return vmin * 0.9, vmax * 1.1

    return vmin * factor_low, vmax * factor_high


def plot_final_state(data):
    """График 1: финальное состояние (ближайшее к T_MAX)"""
    if not data:
        print("  No data to plot")
        return

    times = sorted(data.keys())
    # Ищем ближайшее к T_MAX время
    final_t = min(times, key=lambda x: abs(x - T_MAX))
    df = data[final_t]

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    # Плотность
    axes[0].plot(df['x'], df['rho'], 'b-', linewidth=2)
    axes[0].set_xlabel('x (м)')
    axes[0].set_ylabel('ρ (кг/м³)')
    axes[0].set_title(f'Плотность, t={final_t:.4f} с')
    axes[0].grid(True, alpha=0.3)
    axes[0].set_xlim(X_LEFT, X_RIGHT)

    # Скорость
    axes[1].plot(df['x'], df['u'], 'r-', linewidth=2)
    axes[1].set_xlabel('x (м)')
    axes[1].set_ylabel('u (м/с)')
    axes[1].set_title(f'Скорость, t={final_t:.4f} с')
    axes[1].grid(True, alpha=0.3)
    axes[1].set_xlim(X_LEFT, X_RIGHT)

    # Давление
    axes[2].plot(df['x'], df['P'], 'g-', linewidth=2)
    axes[2].set_xlabel('x (м)')
    axes[2].set_ylabel('P (Па)')
    axes[2].set_title(f'Давление, t={final_t:.4f} с')
    axes[2].grid(True, alpha=0.3)
    axes[2].set_xlim(X_LEFT, X_RIGHT)

    plt.tight_layout()
    plt.savefig(plots_dir / 'final_state.png', dpi=150)
    plt.savefig(plots_dir / 'final_state.pdf')
    plt.close()
    print(f"  Saved final_state.png")


def plot_time_evolution(data):
    """График 2: эволюция во времени (несколько моментов)"""
    if not data:
        return

    times = sorted(data.keys())
    if len(times) < 2:
        print("  Not enough time steps for evolution plot")
        return

    # Выбираем до 5 моментов времени
    n_times = min(5, len(times))
    indices = np.linspace(0, len(times) - 1, n_times, dtype=int)
    selected_times = [times[i] for i in indices]

    colors = plt.cm.viridis(np.linspace(0, 1, len(selected_times)))

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    for idx, t in enumerate(selected_times):
        df = data[t]

        # Плотность
        axes[0].plot(df['x'], df['rho'], color=colors[idx],
                     linewidth=2, label=f't={t:.4f}')

        # Скорость
        axes[1].plot(df['x'], df['u'], color=colors[idx],
                     linewidth=2, label=f't={t:.4f}')

        # Давление
        axes[2].plot(df['x'], df['P'], color=colors[idx],
                     linewidth=2, label=f't={t:.4f}')

    for ax in axes:
        ax.set_xlabel('x (м)')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(X_LEFT, X_RIGHT)
        ax.legend(loc='best')

    axes[0].set_ylabel('ρ (кг/м³)')
    axes[0].set_title('Плотность')

    axes[1].set_ylabel('u (м/с)')
    axes[1].set_title('Скорость')

    axes[2].set_ylabel('P (Па)')
    axes[2].set_title('Давление')

    plt.tight_layout()
    plt.savefig(plots_dir / 'time_evolution.png', dpi=150)
    plt.savefig(plots_dir / 'time_evolution.pdf')
    plt.close()
    print(f"  Saved time_evolution.png")


def plot_comparison_with_initial(data):
    """График 3: сравнение начального и конечного состояния"""
    if not data:
        return

    times = sorted(data.keys())

    # Начальное время (ближайшее к 0)
    t_init = min(times, key=lambda x: abs(x - 0.0))
    # Конечное время (ближайшее к T_MAX)
    t_final = min(times, key=lambda x: abs(x - T_MAX))

    if t_init == t_final:
        print("  Only one time point, skipping comparison")
        return

    df_init = data[t_init]
    df_final = data[t_final]

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    # Плотность
    axes[0].plot(df_init['x'], df_init['rho'], 'b--', linewidth=2, label=f't={t_init:.4f}')
    axes[0].plot(df_final['x'], df_final['rho'], 'r-', linewidth=2, label=f't={t_final:.4f}')
    axes[0].set_xlabel('x (м)')
    axes[0].set_ylabel('ρ (кг/м³)')
    axes[0].set_title('Плотность')
    axes[0].grid(True, alpha=0.3)
    axes[0].legend()
    axes[0].set_xlim(X_LEFT, X_RIGHT)

    # Скорость
    axes[1].plot(df_init['x'], df_init['u'], 'b--', linewidth=2, label=f't={t_init:.4f}')
    axes[1].plot(df_final['x'], df_final['u'], 'r-', linewidth=2, label=f't={t_final:.4f}')
    axes[1].set_xlabel('x (м)')
    axes[1].set_ylabel('u (м/с)')
    axes[1].set_title('Скорость')
    axes[1].grid(True, alpha=0.3)
    axes[1].legend()
    axes[1].set_xlim(X_LEFT, X_RIGHT)
    axes[1].set_ylim(-1, 1)  # фиксированный диапазон для скорости

    # Давление
    axes[2].plot(df_init['x'], df_init['P'], 'b--', linewidth=2, label=f't={t_init:.4f}')
    axes[2].plot(df_final['x'], df_final['P'], 'r-', linewidth=2, label=f't={t_final:.4f}')
    axes[2].set_xlabel('x (м)')
    axes[2].set_ylabel('P (Па)')
    axes[2].set_title('Давление')
    axes[2].grid(True, alpha=0.3)
    axes[2].legend()
    axes[2].set_xlim(X_LEFT, X_RIGHT)

    plt.tight_layout()
    plt.savefig(plots_dir / 'initial_vs_final.png', dpi=150)
    plt.savefig(plots_dir / 'initial_vs_final.pdf')
    plt.close()
    print(f"  Saved initial_vs_final.png")


def create_animation(data):
    """Анимация: эволюция всех переменных во времени"""
    if not data:
        return

    times = sorted(data.keys())
    if len(times) < 2:
        print("  Not enough time steps for animation")
        return

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    # Линии для каждого графика
    line_rho, = axes[0].plot([], [], 'b-', linewidth=2)
    line_u, = axes[1].plot([], [], 'r-', linewidth=2)
    line_P, = axes[2].plot([], [], 'g-', linewidth=2)

    # Определяем диапазоны для осей Y (безопасно)
    all_rho = np.concatenate([data[t]['rho'].values for t in times])
    all_u = np.concatenate([data[t]['u'].values for t in times])
    all_P = np.concatenate([data[t]['P'].values for t in times])

    rho_lims = safe_limits(all_rho)
    u_lims = (-1, 1)  # фиксированный диапазон для скорости
    P_lims = safe_limits(all_P)

    # Настройка графиков
    axes[0].set_xlim(X_LEFT, X_RIGHT)
    axes[0].set_ylim(rho_lims)
    axes[0].set_xlabel('x (м)')
    axes[0].set_ylabel('ρ (кг/м³)')
    axes[0].set_title('Плотность')
    axes[0].grid(True, alpha=0.3)

    axes[1].set_xlim(X_LEFT, X_RIGHT)
    axes[1].set_ylim(u_lims)
    axes[1].set_xlabel('x (м)')
    axes[1].set_ylabel('u (м/с)')
    axes[1].set_title('Скорость')
    axes[1].grid(True, alpha=0.3)

    axes[2].set_xlim(X_LEFT, X_RIGHT)
    axes[2].set_ylim(P_lims)
    axes[2].set_xlabel('x (м)')
    axes[2].set_ylabel('P (Па)')
    axes[2].set_title('Давление')
    axes[2].grid(True, alpha=0.3)

    # Текст с временем
    time_text = fig.suptitle('', fontsize=14)

    def init():
        line_rho.set_data([], [])
        line_u.set_data([], [])
        line_P.set_data([], [])
        time_text.set_text('')
        return line_rho, line_u, line_P, time_text

    def update(frame):
        t = times[frame]
        df = data[t]

        line_rho.set_data(df['x'], df['rho'])
        line_u.set_data(df['x'], df['u'])
        line_P.set_data(df['x'], df['P'])
        time_text.set_text(f't = {t:.6f} с')

        return line_rho, line_u, line_P, time_text

    anim = FuncAnimation(fig, update, frames=len(times),
                         init_func=init, blit=True, interval=50)

    anim.save(plots_dir / 'gas_evolution.gif', writer=PillowWriter(fps=20))
    plt.close(fig)
    print(f"  Saved gas_evolution.gif")


# Основная часть
print("Loading data from gas_data/...")
data = load_data()

if not data:
    print("No data found! Run data_generator.cpp first.")
    print("Check if gas_data/ folder exists and contains CSV files.")
    exit(1)

print(f"\nFound {len(data)} time steps")
print("Creating plots...")

# Создаем все графики
plot_final_state(data)
plot_time_evolution(data)
plot_comparison_with_initial(data)
create_animation(data)

print(f"\nAll plots saved in '{plots_dir}/'")