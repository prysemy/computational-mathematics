import numpy as np
import matplotlib.pyplot as plt
import os


OUTPUT_DIR = "output_2d"
PLOTS_DIR = "plots_2d"
lambda_val = 1e-4

os.makedirs(PLOTS_DIR, exist_ok=True)


def load_slice(filepath):
    x, phi_num, phi_anal = [], [], []
    t, y_mid = None, None

    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line: continue

            if line.startswith('#'):
                if 't =' in line:
                    t = float(line.split('t =')[1].split(',')[0].strip())
                if 'y =' in line:
                    y_mid = float(line.split('y =')[1].strip())
            else:
                parts = line.split()
                if len(parts) >= 3:
                    x.append(float(parts[0]))
                    phi_num.append(float(parts[1]))
                    phi_anal.append(float(parts[2]))

    return np.array(x), np.array(phi_num), np.array(phi_anal), t, y_mid


def load_convergence_data():
    filepath = os.path.join(OUTPUT_DIR, "convergence_data.txt")
    if not os.path.exists(filepath):
        print(f"⚠ Нет файла {filepath}")
        return None, None, None

    Nx, h, err = [], [], []
    with open(filepath, 'r') as f:
        for line in f:
            if line.startswith('#'): continue
            parts = line.split()
            if len(parts) >= 3:
                Nx.append(int(parts[0]))
                h.append(float(parts[1]))
                err.append(float(parts[2]))

    return np.array(Nx), np.array(h), np.array(err)


def plot_slice(Nx=100):
    fname = f"output_2d/slice_N{Nx}.dat"
    if not os.path.exists(fname):
        print(f"⚠ Нет файла {fname}")
        return

    x, num, anal, t, y_mid = load_slice(fname)

    plt.figure(figsize=(10, 6))
    plt.plot(x, num, 'b-', linewidth=2.5, label='Численное')
    plt.plot(x, anal, 'r--', linewidth=2, alpha=0.8, label='Аналитическое')

    plt.xlabel('x', fontsize=12)
    plt.ylabel('φ', fontsize=12)
    plt.title(f'Сечение при y = {y_mid:.1f}, t = {t:.3f}, N = {Nx}',
              fontsize=13, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(fontsize=11)

    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, f'slice_N{Nx}.png'), dpi=300)
    plt.close()
    print(f"✓ slice_N{Nx}.png")


def plot_convergence():
    Nx, h, err = load_convergence_data()
    if Nx is None:
        return

    mask = err > 0
    h_plot = h[mask]
    err_plot = err[mask]
    Nx_plot = Nx[mask]

    plt.figure(figsize=(10, 6))

    # 1. Численная ошибка
    plt.loglog(h_plot, err_plot, 'bo-', linewidth=2, markersize=8,
               label='Численная ошибка', markerfacecolor='blue')

    # 2. Теоретическая зависимость O(h^2)
    # Нормируем по первой точке: err_theory = C * h²
    C = err_plot[0] / (h_plot[0] ** 2)
    h_theory = np.array([h_plot[0], h_plot[-1]])
    err_theory = C * h_theory ** 2
    plt.loglog(h_theory, err_theory, 'r--', linewidth=2.5,
               label='O(h²) — теоретическая сходимость')

    # 3. Расчёт порядка сходимости
    orders = []
    for i in range(1, len(h_plot)):
        p = np.log(err_plot[i - 1] / err_plot[i]) / np.log(h_plot[i - 1] / h_plot[i])
        orders.append(p)

    if orders:
        p_mean = np.mean(orders)
        p_std = np.std(orders)

        info = (f'Порядок сходимости:\n'
                f'численный: {p_mean:.2f} ± {p_std:.2f}\n'
                f'теоретический: 2.00')

        plt.text(0.95, 0.05, info, transform=plt.gca().transAxes,
                 fontsize=10, verticalalignment='bottom', horizontalalignment='right',
                 bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

        print(f"\n📊 Порядок сходимости: {p_mean:.3f} ± {p_std:.3f}")

    plt.xlabel('Шаг по пространству $h$', fontsize=12)
    plt.ylabel('L₂-ошибка при $t = 0.01$', fontsize=12)
    plt.title('Исследование сходимости по пространству',
              fontsize=13, fontweight='bold')
    plt.grid(True, alpha=0.3, which='both')
    plt.legend(fontsize=11)

    for xi, yi, ni in zip(h_plot, err_plot, Nx_plot):
        plt.annotate(f'N={ni}', (xi, yi), textcoords="offset points",
                     xytext=(5, -8), fontsize=8, alpha=0.7)

    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, 'convergence.png'), dpi=300)
    plt.close()
    print("✓ convergence.png")


def plot_error_table():
    Nx, h, err = load_convergence_data()
    if Nx is None:
        return

    print("\n" + "=" * 60)
    print("📋 ТАБЛИЦА СХОДИМОСТИ")
    print("=" * 60)
    print(f"{'N':>6} {'h':>12} {'Ошибка (L₂)':>18} {'Отношение':>12}")
    print("-" * 60)

    prev_err = None
    for i in range(len(Nx)):
        ratio = ""
        if prev_err is not None and err[i] > 0:
            ratio = f"{prev_err / err[i]:.2f}×"
        print(f"{Nx[i]:6d} {h[i]:12.6f} {err[i]:18.3e} {ratio:>12}")
        prev_err = err[i]

    print("=" * 60)

    with open(os.path.join(PLOTS_DIR, 'convergence_table.txt'), 'w') as f:
        f.write("N, h, L2_error\n")
        for i in range(len(Nx)):
            f.write(f"{Nx[i]}, {h[i]}, {err[i]}\n")
    print(f"Таблица сохранена: {PLOTS_DIR}/convergence_table.txt")


def main():
    print("=" * 60)
    print("Жоска делаю графики, рокстар бейби")
    print("=" * 60)

    print("\nПостроение графиков...")
    plot_slice(Nx=100)

    plot_convergence()

    plot_error_table()

    print(f"\nГотово! Графики в папке: {PLOTS_DIR}/")


if __name__ == "__main__":
    main()