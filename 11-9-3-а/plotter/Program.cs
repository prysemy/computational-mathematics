using System;
using System.Linq;
using ScottPlot;

class Program
{
    static void Main(string[] args)
    {
        var plt = new Plot();
        plt.XLabel("x");
        plt.YLabel("y");

        // Ось x от 0 до 1
        plt.Axes.SetLimits(-0.1, 1.1, -0.2, 3.0);

        // Целевая точка y(1)=2
        double targetX = 1.0;
        double targetY = 2.0;

        // 1. Разные "выстрелы" с разными начальными производными
        // Выстрел 1: слишком малый угол (недолет)
        double alpha1 = 1.5; // y'(0) = 1.5
        double[] x1 = { 0.0, 0.2, 0.4, 0.6, 0.8, 1.0 };
        double[] y1 = x1.Select(x => alpha1 * x + 0.1 * x * x).ToArray(); // Примерная траектория

        var shot1 = plt.Add.Scatter(x1, y1);
        shot1.Color = ScottPlot.Color.FromHex("#ff7f0e").WithAlpha(0.6f); // Оранжевый
        shot1.LineWidth = 2;

        // Выстрел 2: слишком большой угол (перелет)
        double alpha2 = 3.0; // y'(0) = 3.0
        double[] y2 = x1.Select(x => alpha2 * x - 0.2 * x * x).ToArray(); // Примерная траектория

        var shot2 = plt.Add.Scatter(x1, y2);
        shot2.Color = ScottPlot.Color.FromHex("#d62728").WithAlpha(0.6f); // Красный
        shot2.LineWidth = 2;

        // Выстрел 3: правильный угол (попадание)
        double alpha3 = 2.1; // Примерное правильное значение
        double[] y3 = x1.Select(x => alpha3 * x - 0.1163 * x * x).ToArray(); // Близко к реальному решению

        var shot3 = plt.Add.Scatter(x1, y3);
        shot3.Color = ScottPlot.Color.FromHex("#2ca02c"); // Зеленый
        shot3.LineWidth = 3;

        // 2. Начальная точка (0,0)
        var startPoint = plt.Add.Marker(0.0, 0.0, ScottPlot.MarkerShape.FilledCircle, 10,
                                       ScottPlot.Color.FromHex("#1f77b4"));

        // 3. Целевая точка (1,2)
        var targetPoint = plt.Add.Marker(targetX, targetY, ScottPlot.MarkerShape.FilledCircle, 10,
                                        ScottPlot.Color.FromHex("#ff0000"));

        // 4. Стрелки, показывающие направление "выстрелов"
        var arrow1 = plt.Add.Arrow(0.05, 0.075, 0.2, alpha1 * 0.2);
        var arrow2 = plt.Add.Arrow(0.05, 0.15, 0.2, alpha2 * 0.2);
        var arrow3 = plt.Add.Arrow(0.05, 0.133, 0.2, alpha3 * 0.2);

        string filename = "shooting_method_diagram.png";
        plt.SavePng(filename, 800, 600);
        Console.WriteLine($"Created: {filename}");
    }
}