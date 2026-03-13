using System;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Collections.Generic;
using ScottPlot;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("=== Plotting Runge-Kutta 5th Order Solution ===");
        string dataFile = "../data_generator/solution.csv";

        if (!File.Exists(dataFile))
        {
            Console.WriteLine("Data file not found! Run C++ data generator first.");
            return;
        }

        try
        {
            var lines = File.ReadAllLines(dataFile);

            // Пропускаем заголовок
            var dataLines = lines.Skip(1).Where(line => !string.IsNullOrWhiteSpace(line)).ToArray();

            var dataPoints = ParseDataFile(dataLines);

            CreatePlots(dataPoints);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            Console.WriteLine($"StackTrace: {ex.StackTrace}");
        }
    }

    static List<DataPoint> ParseDataFile(string[] lines)
    {
        var dataPoints = new List<DataPoint>();

        foreach (var line in lines)
        {
            var parts = line.Split(',');

            if (parts.Length >= 6)
            {
                var point = new DataPoint
                {
                    x = double.Parse(parts[0], CultureInfo.InvariantCulture),
                    y_adaptive = double.Parse(parts[1], CultureInfo.InvariantCulture),
                    y_fixed = double.Parse(parts[2], CultureInfo.InvariantCulture),
                    y_exact = double.Parse(parts[3], CultureInfo.InvariantCulture),
                    error_adaptive = double.Parse(parts[4], CultureInfo.InvariantCulture),
                    error_fixed = double.Parse(parts[5], CultureInfo.InvariantCulture)
                };

                dataPoints.Add(point);
            }
        }

        return dataPoints;
    }

    static void CreatePlots(List<DataPoint> dataPoints)
    {
        CreateFullSolutionPlot(dataPoints);

        CreateInitialSolutionPlot(dataPoints);

        CreateErrorPlot(dataPoints);
    }

    static void CreateFullSolutionPlot(List<DataPoint> dataPoints)
    {
        var plt = new Plot();
        plt.Title("Решение уравнения y' = -2y методом Рунге-Кутты 5-го порядка", size: 16);
        plt.XLabel("x");
        plt.YLabel("y(x)");

        var x = dataPoints.Select(p => p.x).ToArray();
        var y_exact = dataPoints.Select(p => p.y_exact).ToArray();
        var y_adaptive = dataPoints.Select(p => p.y_adaptive).ToArray();
        var y_fixed = dataPoints.Select(p => p.y_fixed).ToArray();

        // Точное решение
        var exactLine = plt.Add.Scatter(x, y_exact);
        exactLine.Color = ScottPlot.Color.FromHex("#1f77b4"); // Blue
        exactLine.LineWidth = 2;
        exactLine.Label = "Точное решение: y = -2e^{-2x}";

        // Решение с адаптивным шагом
        var adaptiveLine = plt.Add.Scatter(x, y_adaptive);
        adaptiveLine.Color = ScottPlot.Color.FromHex("#ff7f0e"); // Orange
        adaptiveLine.LineWidth = 1.5f;
        adaptiveLine.MarkerSize = 3;
        adaptiveLine.Label = "Адаптивный шаг";

        // Решение с фиксированным шагом
        var fixedLine = plt.Add.Scatter(x, y_fixed);
        fixedLine.Color = ScottPlot.Color.FromHex("#2ca02c"); // Green
        fixedLine.LineWidth = 1.5f;
        fixedLine.MarkerSize = 3;
        fixedLine.Label = "Фиксированный шаг (h=0.01)";

        // Горизонтальная линия y = 0
        var zeroLine = plt.Add.HorizontalLine(0);
        zeroLine.Color = ScottPlot.Color.FromHex("#7f7f7f").WithAlpha(0.5f);
        zeroLine.LineWidth = 1;

        string filename = "full_solution.png";
        plt.SavePng(filename, 1200, 800);
    }

    static void CreateInitialSolutionPlot(List<DataPoint> dataPoints)
    {
        // Берем только точки на интервале [0, 5]
        var initialData = dataPoints.Where(p => p.x <= 5).ToList();

        var plt = new Plot();
        plt.Title("Начальный участок решения (x ∈ [0, 5])", size: 16);
        plt.XLabel("x");
        plt.YLabel("y(x)");

        var x = initialData.Select(p => p.x).ToArray();
        var y_exact = initialData.Select(p => p.y_exact).ToArray();
        var y_adaptive = initialData.Select(p => p.y_adaptive).ToArray();
        var y_fixed = initialData.Select(p => p.y_fixed).ToArray();

        // Точное решение
        var exactLine = plt.Add.Scatter(x, y_exact);
        exactLine.Color = ScottPlot.Color.FromHex("#1f77b4");
        exactLine.LineWidth = 3;
        exactLine.Label = "Точное решение";

        // Решение с адаптивным шагом (точками)
        var adaptiveScatter = plt.Add.Scatter(x, y_adaptive);
        adaptiveScatter.Color = ScottPlot.Color.FromHex("#ff7f0e");
        adaptiveScatter.MarkerSize = 5;
        adaptiveScatter.LineWidth = 0;
        adaptiveScatter.Label = "Адаптивный шаг (точки)";

        // Решение с фиксированным шагом
        var fixedLine = plt.Add.Scatter(x, y_fixed);
        fixedLine.Color = ScottPlot.Color.FromHex("#2ca02c");
        fixedLine.LineWidth = 1.5f;
        fixedLine.Label = "Фиксированный шаг";

        string filename = "initial_solution.png";
        plt.SavePng(filename, 1000, 700);
    }

    static void CreateErrorPlot(List<DataPoint> dataPoints)
    {
        var plt = new Plot();
        plt.Title("Погрешности численных решений", size: 16);
        plt.XLabel("x");
        plt.YLabel("Абсолютная погрешность |Δy|");

        var x = dataPoints.Select(p => p.x).ToArray();
        var error_adaptive = dataPoints.Select(p => p.error_adaptive).ToArray();
        var error_fixed = dataPoints.Select(p => p.error_fixed).ToArray();

        // Погрешность адаптивного метода
        var adaptiveErrorLine = plt.Add.Scatter(x, error_adaptive);
        adaptiveErrorLine.Color = ScottPlot.Color.FromHex("#ff7f0e");
        adaptiveErrorLine.LineWidth = 2;
        adaptiveErrorLine.Label = "Адаптивный шаг";

        // Погрешность фиксированного шага
        var fixedErrorLine = plt.Add.Scatter(x, error_fixed);
        fixedErrorLine.Color = ScottPlot.Color.FromHex("#2ca02c");
        fixedErrorLine.LineWidth = 2;
        fixedErrorLine.Label = "Фиксированный шаг";

        // Горизонтальная линия для уровня точности 1e-8
        var toleranceLine = plt.Add.HorizontalLine(1e-8);
        toleranceLine.Color = ScottPlot.Color.FromHex("#1f77b4").WithAlpha(0.7f);
        toleranceLine.LineWidth = 2;

        string filename = "errors_log.png";
        plt.SavePng(filename, 1200, 800);
    }
}

public class DataPoint
{
    public double x { get; set; }
    public double y_adaptive { get; set; }
    public double y_fixed { get; set; }
    public double y_exact { get; set; }
    public double error_adaptive { get; set; }
    public double error_fixed { get; set; }
}