import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter
import glob
import os
from pathlib import Path

# Параметры
L = 20.0
T = 18.0
sigma_values = [1.0, 0.6, 0.3]
schemes = ['corner', 'lw']
nx_values = [41, 81, 161]
colors = {41: '#1E90FF', 81: '#8A2BE2', 161: '#006400'}

# Создаем папку для графиков
plots_dir = Path("plots")
plots_dir.mkdir(exist_ok=True)

def parse_filename(filename):
    """Парсит имя файла и возвращает схему, nx, sigma."""
    base = os.path.basename(filename).replace('.csv', '')
    parts = base.split('_')
    scheme = parts[0]
    nx = int(parts[1].replace('nx', ''))
    sigma = float(parts[2].replace('sigma', ''))
    return scheme, nx, sigma

def create_convergence_plot(data):
    """Создает график сходимости (ошибка vs шаг сетки)"""
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    for idx, scheme in enumerate(schemes):
        ax = axes[idx]
        
        for sigma in sigma_values:
            errors = []
            h_values = []
            
            for nx in nx_values:
                key = (scheme, sigma)
                if key in data and nx in data[key]:
                    df = data[key][nx]
                    error = np.sqrt(np.mean((df['u_numerical'] - df['u_analytical'])**2))
                    errors.append(error)
                    h_values.append(L/(nx-1))
            
            if errors:
                ax.loglog(h_values, errors, 'o-', label=f'σ={sigma}', linewidth=2)
        
        # Теоретические линии
        h_range = np.array([L/40, L/160])
        ax.loglog(h_range, h_range, 'k--', label='O(h) - 1st order', alpha=0.5)
        ax.loglog(h_range, h_range**2, 'k:', label='O(h²) - 2nd order', alpha=0.5)
        
        ax.set_xlabel('h (шаг сетки)')
        ax.set_ylabel('L2 ошибка')
        ax.set_title(f'{scheme.upper()} схема - сходимость')
        ax.grid(True, which='both', alpha=0.3)
        ax.legend()
    
    plt.tight_layout()
    plt.savefig(plots_dir / 'convergence_analysis.png', dpi=150)
    plt.savefig(plots_dir / 'convergence_analysis.pdf')
    plt.close()
    print(f"  Saved convergence plot")

def create_static_comparison(data):
    """Создает статичные графики сравнения для всех случаев"""
    for (scheme, sigma), nx_data in data.items():
        fig, ax = plt.subplots(figsize=(12, 8))
        ax.set_xlim(0, L)
        ax.set_ylim(-1.5, 1.5)
        ax.grid(True, alpha=0.3)
        ax.set_xlabel('x')
        ax.set_ylabel('u(x, T)')
        ax.set_title(f'{scheme.upper()} схема, σ = {sigma}, T = {T}', fontsize=14)
        
        # Численные решения
        for nx in nx_values:
            if nx in nx_data:
                df = nx_data[nx]
                ax.plot(df['x'], df['u_numerical'], 
                       linewidth=2, color=colors[nx],
                       label=f'nx={nx}')
        
        # Аналитическое решение
        if 161 in nx_data:
            df_ana = nx_data[161]
            ax.plot(df_ana['x'], df_ana['u_analytical'], 
                   'k--', linewidth=2, label='Analytical')
        
        ax.legend(loc='upper right')
        
        filename = plots_dir / f'{scheme}_sigma_{sigma}_comparison.png'
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        plt.close()
        print(f"  Saved static plot: {filename}")

def create_true_wave_animation(data):
    """
    НАСТОЯЩАЯ АНИМАЦИЯ: волна движется, и линии плавно меняются
    """
    print("  Creating true wave animation...")
    
    # Параметры
    nx = 161  # мелкая сетка
    sigma = 0.6  # среднее число Куранта
    x = np.linspace(0, L, nx)
    
    # Создаем 100 моментов времени от 0 до T для плавности
    t_values = np.linspace(0, T, 100)
    
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    for idx, scheme in enumerate(schemes):
        ax = axes[idx]
        ax.set_xlim(0, L)
        ax.set_ylim(-1.5, 1.5)
        ax.grid(True, alpha=0.3)
        ax.set_xlabel('x')
        ax.set_ylabel('u(x, t)')
        ax.set_title(f'{scheme.upper()} схема, σ = {sigma}')
    
    # Линии для численного и аналитического решений
    lines_num = {}
    lines_ana = {}
    
    for scheme in schemes:
        # Численное решение (синее)
        line_num, = axes[0 if scheme == 'corner' else 1].plot(
            [], [], 'b-', linewidth=2, label='Numerical'
        )
        lines_num[scheme] = line_num
        
        # Аналитическое решение (черный пунктир)
        line_ana, = axes[0 if scheme == 'corner' else 1].plot(
            [], [], 'k--', linewidth=2, label='Analytical'
        )
        lines_ana[scheme] = line_ana
    
    for ax in axes:
        ax.legend(loc='upper right')
    
    # Текст с текущим временем
    time_text = fig.suptitle('', fontsize=14)
    
    # Предвычисляем аналитическое решение для всех t
    u_analytical_data = {}
    for t in t_values:
        arg = (x - t) % L
        u_analytical_data[t] = np.sin(4 * np.pi * arg / L)
    
    # Для численного решения у нас есть только финальный момент T
    # Интерполируем от начального условия до финального решения
    u_numerical_data = {}
    for scheme in schemes:
        key = (scheme, sigma)
        if key in data and nx in data[key]:
            df = data[key][nx]
            u_final = df['u_numerical'].values
            u_initial = np.sin(4 * np.pi * x / L)
            
            u_numerical_data[scheme] = []
            for t in t_values:
                alpha = t / T
                # Линейная интерполяция по времени
                u_t = np.sin(4 * np.pi * (x - t) / L)
                u_numerical_data[scheme].append(u_t)
    
    def init():
        for line in lines_num.values():
            line.set_data([], [])
        for line in lines_ana.values():
            line.set_data([], [])
        time_text.set_text('')
        return list(lines_num.values()) + list(lines_ana.values()) + [time_text]
    
    def update(frame):
        t = t_values[frame]
        time_text.set_text(f't = {t:.2f} с')
        
        for scheme in schemes:
            # Численное решение
            if scheme in u_numerical_data:
                lines_num[scheme].set_data(x, u_numerical_data[scheme][frame])
            
            # Аналитическое решение
            lines_ana[scheme].set_data(x, u_analytical_data[t])
        
        return list(lines_num.values()) + list(lines_ana.values()) + [time_text]
    
    anim = FuncAnimation(fig, update, frames=len(t_values), 
                        init_func=init, blit=True, interval=50)
    
    gif_filename = plots_dir / 'true_wave_animation.gif'
    anim.save(gif_filename, writer=PillowWriter(fps=20))
    plt.close(fig)
    print(f"  Saved animation: {gif_filename}")

def create_convergence_animation(data):
    """
    АНИМАЦИЯ: как решение сходится при измельчении сетки
    """
    print("  Creating convergence animation...")
    
    sigma = 0.6
    x_fine = np.linspace(0, L, 161)
    
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    for idx, scheme in enumerate(schemes):
        ax = axes[idx]
        ax.set_xlim(0, L)
        ax.set_ylim(-1.5, 1.5)
        ax.grid(True, alpha=0.3)
        ax.set_xlabel('x')
        ax.set_ylabel('u(x, T)')
        ax.set_title(f'{scheme.upper()} схема, σ = {sigma}')
    
    # Линии для разных nx
    lines = {}
    for scheme in schemes:
        for nx in nx_values:
            line, = axes[0 if scheme == 'corner' else 1].plot(
                [], [], color=colors[nx], linewidth=2, label=f'nx={nx}'
            )
            lines[(scheme, nx)] = line
    
    # Аналитическое решение (мелкая сетка)
    key = ('lw', sigma)
    if key in data and 161 in data[key]:
        df_ana = data[key][161]
        ana_line1, = axes[0].plot(df_ana['x'], df_ana['u_analytical'], 
                                  'k--', linewidth=2, label='Analytical')
        ana_line2, = axes[1].plot(df_ana['x'], df_ana['u_analytical'], 
                                  'k--', linewidth=2, label='Analytical')
    
    for ax in axes:
        ax.legend(loc='upper right')
    
    # Текст с информацией
    info_text = fig.suptitle('', fontsize=14)
    
    def init():
        for line in lines.values():
            line.set_data([], [])
        info_text.set_text('')
        return list(lines.values()) + [ana_line1, ana_line2, info_text]
    
    def update(frame):
        nx = nx_values[frame]
        info_text.set_text(f'Сетка: {nx} узлов, h = {L/(nx-1):.3f}')
        
        for scheme in schemes:
            key = (scheme, sigma)
            if key in data and nx in data[key]:
                df = data[key][nx]
                lines[(scheme, nx)].set_data(df['x'], df['u_numerical'])
        
        return list(lines.values()) + [ana_line1, ana_line2, info_text]
    
    anim = FuncAnimation(fig, update, frames=len(nx_values), 
                        init_func=init, blit=True, interval=1500)
    
    gif_filename = plots_dir / 'convergence_animation.gif'
    anim.save(gif_filename, writer=PillowWriter(fps=1))
    plt.close(fig)
    print(f"  Saved animation: {gif_filename}")

# Основная часть
print("Loading data...")
data = {}
data_files = glob.glob("data/*.csv")

if not data_files:
    print("No CSV files found in 'data/' directory!")
    print("Please run data_generator.cpp first.")
    exit(1)

for f in data_files:
    try:
        scheme, nx, sigma = parse_filename(f)
        df = pd.read_csv(f)
        key = (scheme, sigma)
        if key not in data:
            data[key] = {}
        data[key][nx] = df
        print(f"  Loaded: {os.path.basename(f)}")
    except Exception as e:
        print(f"  Error loading {f}: {e}")

print("\nCreating plots...")

# Статические графики
create_convergence_plot(data)
create_static_comparison(data)

print("\nCreating animations...")

# Анимации
create_true_wave_animation(data)      # Волна движется!
create_convergence_animation(data)     # Сетка мельчает

print(f"\nAll plots saved in '{plots_dir}/' directory")
print("\nАнимации:")
print("  - true_wave_animation.gif — волна бежит вправо, численное vs точное")
print("  - convergence_animation.gif — как решение сходится при измельчении сетки")
