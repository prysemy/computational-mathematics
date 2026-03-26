import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Чтение данных
df = pd.read_csv('results2.csv')
times = sorted(df['t'].unique())

print(f"Всего кадров: {len(times)}")
print(f"Время: {times[0]:.4f} - {times[-1]:.4f} с")

# Создаем папку для графиков
os.makedirs("gas_plots", exist_ok=True)

# Весь расчетный отрезок от -10 до 10
x_min, x_max = -10, 10

# ==================== ГРАФИК 1: ФИНАЛЬНОЕ СОСТОЯНИЕ (4 графика) ====================
t_final = times[-1]
data_final = df[df['t'] == t_final]

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle(f'Задача о распаде разрыва (задача Сода)\nγ = 5/3, t = {t_final:.4f} с',
             fontsize=14, fontweight='bold')

# 1. Плотность
ax = axes[0, 0]
ax.plot(data_final['x'], data_final['rho'], 'b-', linewidth=2)
ax.axhline(y=13, color='gray', linestyle='--', alpha=0.5, label='ρ_L = 13')
ax.axhline(y=1.3, color='gray', linestyle='--', alpha=0.5, label='ρ_R = 1.3')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('ρ, кг/м³', fontsize=12)
ax.set_title('Плотность', fontsize=12)
ax.grid(True, alpha=0.3)
ax.set_xlim(x_min, x_max)
ax.legend()

# 2. Скорость
ax = axes[0, 1]
ax.plot(data_final['x'], data_final['u'], 'r-', linewidth=2)
ax.axhline(y=0, color='gray', linestyle='--', alpha=0.5)
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('u, м/с', fontsize=12)
ax.set_title('Скорость', fontsize=12)
ax.grid(True, alpha=0.3)
ax.set_xlim(x_min, x_max)

# 3. Давление
ax = axes[1, 0]
ax.plot(data_final['x'], data_final['P'], 'g-', linewidth=2)
ax.axhline(y=10, color='gray', linestyle='--', alpha=0.5, label='P_L = 10')
ax.axhline(y=1, color='gray', linestyle='--', alpha=0.5, label='P_R = 1')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('P, атм', fontsize=12)
ax.set_title('Давление', fontsize=12)
ax.grid(True, alpha=0.3)
ax.set_xlim(x_min, x_max)
ax.legend()

# 4. Внутренняя энергия
ax = axes[1, 1]
ax.plot(data_final['x'], data_final['e'], 'm-', linewidth=2)
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('e, Дж/кг', fontsize=12)
ax.set_title('Внутренняя энергия', fontsize=12)
ax.grid(True, alpha=0.3)
ax.set_xlim(x_min, x_max)

plt.tight_layout()
plt.savefig('gas_plots/final_state.png', dpi=200, bbox_inches='tight')
plt.savefig('gas_plots/final_state.pdf', bbox_inches='tight')
print("✓ Финальное состояние сохранено")

# ==================== ГРАФИК 2: СРАВНЕНИЕ С НАЧАЛЬНЫМ СОСТОЯНИЕМ ====================
t_init = times[0]
data_init = df[df['t'] == t_init]

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Сравнение начального и конечного состояния', fontsize=14, fontweight='bold')

# Плотность
ax = axes[0, 0]
ax.plot(data_init['x'], data_init['rho'], 'b--', linewidth=2, label=f'Начало, t=0')
ax.plot(data_final['x'], data_final['rho'], 'r-', linewidth=2, label=f'Конец, t={t_final:.3f} с')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('ρ, кг/м³', fontsize=12)
ax.set_title('Плотность', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Скорость
ax = axes[0, 1]
ax.plot(data_init['x'], data_init['u'], 'b--', linewidth=2, label=f'Начало, t=0')
ax.plot(data_final['x'], data_final['u'], 'r-', linewidth=2, label=f'Конец, t={t_final:.3f} с')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('u, м/с', fontsize=12)
ax.set_title('Скорость', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Давление
ax = axes[1, 0]
ax.plot(data_init['x'], data_init['P'], 'b--', linewidth=2, label=f'Начало, t=0')
ax.plot(data_final['x'], data_final['P'], 'r-', linewidth=2, label=f'Конец, t={t_final:.3f} с')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('P, атм', fontsize=12)
ax.set_title('Давление', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Внутренняя энергия
ax = axes[1, 1]
ax.plot(data_init['x'], data_init['e'], 'b--', linewidth=2, label=f'Начало, t=0')
ax.plot(data_final['x'], data_final['e'], 'r-', linewidth=2, label=f'Конец, t={t_final:.3f} с')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('e, Дж/кг', fontsize=12)
ax.set_title('Внутренняя энергия', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

plt.tight_layout()
plt.savefig('gas_plots/initial_vs_final.png', dpi=200, bbox_inches='tight')
print("✓ Сравнение сохранено")

# ==================== ГРАФИК 3: ЭВОЛЮЦИЯ ВО ВРЕМЕНИ (5 моментов) ====================
# Выбираем несколько моментов времени
n_moments = min(5, len(times))
indices = np.linspace(1, len(times) - 1, n_moments, dtype=int)
selected_times = [times[i] for i in indices]
colors = plt.cm.viridis(np.linspace(0, 1, n_moments))

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Эволюция параметров во времени', fontsize=14, fontweight='bold')

for idx, (t, color) in enumerate(zip(selected_times, colors)):
    data_t = df[df['t'] == t]
    axes[0, 0].plot(data_t['x'], data_t['rho'], color=color, linewidth=1.5, label=f't={t:.3f} с')
    axes[0, 1].plot(data_t['x'], data_t['u'], color=color, linewidth=1.5, label=f't={t:.3f} с')
    axes[1, 0].plot(data_t['x'], data_t['P'], color=color, linewidth=1.5, label=f't={t:.3f} с')
    axes[1, 1].plot(data_t['x'], data_t['e'], color=color, linewidth=1.5, label=f't={t:.3f} с')

# Плотность
axes[0, 0].set_xlabel('x, м')
axes[0, 0].set_ylabel('ρ, кг/м³')
axes[0, 0].set_title('Плотность')
axes[0, 0].grid(True, alpha=0.3)
axes[0, 0].set_xlim(x_min, x_max)
axes[0, 0].legend(fontsize=8)

# Скорость
axes[0, 1].set_xlabel('x, м')
axes[0, 1].set_ylabel('u, м/с')
axes[0, 1].set_title('Скорость')
axes[0, 1].grid(True, alpha=0.3)
axes[0, 1].set_xlim(x_min, x_max)
axes[0, 1].legend(fontsize=8)

# Давление
axes[1, 0].set_xlabel('x, м')
axes[1, 0].set_ylabel('P, атм')
axes[1, 0].set_title('Давление')
axes[1, 0].grid(True, alpha=0.3)
axes[1, 0].set_xlim(x_min, x_max)
axes[1, 0].legend(fontsize=8)

# Внутренняя энергия
axes[1, 1].set_xlabel('x, м')
axes[1, 1].set_ylabel('e, Дж/кг')
axes[1, 1].set_title('Внутренняя энергия')
axes[1, 1].grid(True, alpha=0.3)
axes[1, 1].set_xlim(x_min, x_max)
axes[1, 1].legend(fontsize=8)

plt.tight_layout()
plt.savefig('gas_plots/evolution.png', dpi=200, bbox_inches='tight')
print("✓ Эволюция сохранена")

# ==================== ГРАФИК 4: СРАВНЕНИЕ С АНАЛИТИЧЕСКИМ РЕШЕНИЕМ ====================
print("\nСравнение с аналитическим решением...")


# Аналитическое решение для задачи Сода (приближенные значения для γ=5/3)
def analytical_solution(x, t, gamma=5 / 3):
    # Параметры из аналитического решения
    P_star = 1.8  # давление в средней области
    u_star = 0.6  # скорость в средней области
    rho_star_left = 4.5  # плотность слева от контактного разрыва
    rho_star_right = 1.8  # плотность справа от контактного разрыва

    # Скорости звука
    c_left = np.sqrt(gamma * 10 / 13)  # скорость звука слева
    c_right = np.sqrt(gamma * 1 / 1.3)  # скорость звука справа

    # Положения волн
    x_contact = u_star * t
    x_shock = (u_star + c_right * np.sqrt((gamma + 1) / (2 * gamma) * P_star + (gamma - 1) / (2 * gamma))) * t
    x_rarefaction_head = -c_left * t
    x_rarefaction_tail = (u_star - c_left * (P_star / 10) ** ((gamma - 1) / (2 * gamma))) * t

    rho = np.zeros_like(x)
    u = np.zeros_like(x)
    P = np.zeros_like(x)

    for i, xi in enumerate(x):
        if xi < x_rarefaction_head:
            rho[i] = 13
            u[i] = 0
            P[i] = 10
        elif xi < x_rarefaction_tail:
            # Волна разрежения
            rho[i] = 13 * ((1 - (gamma - 1) / (2 * c_left) * (xi - x_rarefaction_head) / t) ** (2 / (gamma - 1)))
            u[i] = (2 / (gamma + 1)) * (c_left + (xi - x_rarefaction_head) / t)
            P[i] = 10 * (rho[i] / 13) ** gamma
        elif xi < x_contact:
            rho[i] = rho_star_left
            u[i] = u_star
            P[i] = P_star
        elif xi < x_shock:
            rho[i] = rho_star_right
            u[i] = u_star
            P[i] = P_star
        else:
            rho[i] = 1.3
            u[i] = 0
            P[i] = 1

    e = P / ((gamma - 1) * rho)
    return rho, u, P, e


rho_anal, u_anal, P_anal, e_anal = analytical_solution(data_final['x'].values, t_final)

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle(f'Сравнение численного и аналитического решения, t = {t_final:.3f} с',
             fontsize=14, fontweight='bold')

# Плотность
ax = axes[0, 0]
ax.plot(data_final['x'], data_final['rho'], 'b-', linewidth=2, label='Численное')
ax.plot(data_final['x'], rho_anal, 'r--', linewidth=2, label='Аналитическое')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('ρ, кг/м³', fontsize=12)
ax.set_title('Плотность', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Скорость
ax = axes[0, 1]
ax.plot(data_final['x'], data_final['u'], 'b-', linewidth=2, label='Численное')
ax.plot(data_final['x'], u_anal, 'r--', linewidth=2, label='Аналитическое')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('u, м/с', fontsize=12)
ax.set_title('Скорость', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Давление
ax = axes[1, 0]
ax.plot(data_final['x'], data_final['P'], 'b-', linewidth=2, label='Численное')
ax.plot(data_final['x'], P_anal, 'r--', linewidth=2, label='Аналитическое')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('P, атм', fontsize=12)
ax.set_title('Давление', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

# Внутренняя энергия
ax = axes[1, 1]
ax.plot(data_final['x'], data_final['e'], 'b-', linewidth=2, label='Численное')
ax.plot(data_final['x'], e_anal, 'r--', linewidth=2, label='Аналитическое')
ax.set_xlabel('x, м', fontsize=12)
ax.set_ylabel('e, Дж/кг', fontsize=12)
ax.set_title('Внутренняя энергия', fontsize=12)
ax.grid(True, alpha=0.3)
ax.legend()
ax.set_xlim(x_min, x_max)

plt.tight_layout()
plt.savefig('gas_plots/numerical_vs_analytical.png', dpi=200, bbox_inches='tight')
print("✓ Сравнение с аналитическим решением сохранено")

print("\n" + "=" * 50)
print("ВСЕ ГРАФИКИ СОХРАНЕНЫ В ПАПКЕ gas_plots/")
print("=" * 50)
print("\nФайлы:")
print("  📊 final_state.png              - финальное состояние")
print("  📊 initial_vs_final.png         - сравнение начала и конца")
print("  📊 evolution.png                - эволюция во времени")
print("  📊 numerical_vs_analytical.png  - сравнение с аналитикой")
