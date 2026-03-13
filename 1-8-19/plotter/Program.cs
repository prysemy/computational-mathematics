using System;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Collections.Generic;
using ScottPlot;

class Program {
    static void Main(string[] args) {
        Console.WriteLine("=== Plotting Data ===");
        string dataFile = "../data_generator/data/results.txt";
        if (!File.Exists(dataFile)) {
            Console.WriteLine("Data file not found! Run data generator first.");
            return;
        }
        try {
            var lines = File.ReadAllLines(dataFile);
            var (data01, data1011, optimalValues) = ParseDataFile(lines);

            CreatePlots(data01, data1011, optimalValues);
        }
        catch (Exception ex) {
            Console.WriteLine($"Error: {ex.Message}");
        }
    }

    static (List<DataPoint> data01, List<DataPoint> data1011, Dictionary<string, int> optimalValues) ParseDataFile(string[] lines) {
        var data01 = new List<DataPoint>();
        var data1011 = new List<DataPoint>();
        var optimalValues = new Dictionary<string, int>();

        bool reading01 = false;
        bool reading1011 = false;

        foreach (var line in lines) {
            if (line.Contains("n =")) {
                var parts = line.Split(new[] { "n = " }, StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length == 2) {
                    var key = parts[0].Trim();
                    var value = int.Parse(parts[1].Trim());
                    optimalValues[key] = value;
                }
            }

            if (line.Contains("Значения функций на [0,1]:")) {
                reading01 = true;
                reading1011 = false;
                continue;
            }
            if (line.Contains("Значения функций на [10,11]:")) {
                reading01 = false;
                reading1011 = true;
                continue;
            }
            if (line.StartsWith("t\tsin_exact") || string.IsNullOrWhiteSpace(line))
                continue;

            var dataParts = line.Split('\t');
            if (dataParts.Length >= 7) {
                var point = new DataPoint {
                    t = double.Parse(dataParts[0], CultureInfo.InvariantCulture),
                    sin_exact = double.Parse(dataParts[1], CultureInfo.InvariantCulture),
                    sin_approx = double.Parse(dataParts[2], CultureInfo.InvariantCulture),
                    exp_exact = double.Parse(dataParts[3], CultureInfo.InvariantCulture),
                    exp_approx = double.Parse(dataParts[4], CultureInfo.InvariantCulture),
                    sin_improved = double.Parse(dataParts[5], CultureInfo.InvariantCulture),
                    exp_improved = double.Parse(dataParts[6], CultureInfo.InvariantCulture)
                };

                if (reading01)
                    data01.Add(point);
                else if (reading1011)
                    data1011.Add(point);
            }
        }

        return (data01, data1011, optimalValues);
    }

    static void CreatePlots(List<DataPoint> data01, List<DataPoint> data1011, Dictionary<string, int> optimalValues) {
        Console.WriteLine("Creating plots for both intervals...");

        int n_sin_01 = optimalValues["sin(t) на [0,1]:"];
        int n_exp_01 = optimalValues["exp(t) на [0,1]:"];
        int n_sin_1011 = optimalValues["sin(t) на [10,11]:"];
        int n_exp_1011 = optimalValues["exp(t) на [10,11]:"];

        Console.WriteLine($"Using n_sin_01 = {n_sin_01}, n_exp_01 = {n_exp_01}");
        Console.WriteLine($"Using n_sin_1011 = {n_sin_1011}, n_exp_1011 = {n_exp_1011}");

        var plt_sin_01 = new Plot();
        var extended_sin_x_01 = new List<double>();
        var extended_sin_y_01 = new List<double>();
        for (double t = -1.0; t <= 2.0; t += 0.01) {
            extended_sin_x_01.Add(t);
            extended_sin_y_01.Add(Math.Sin(t));
        }
        var extendedSinLine01 = plt_sin_01.Add.Scatter(extended_sin_x_01.ToArray(), extended_sin_y_01.ToArray());
        extendedSinLine01.Color = Colors.Gray.WithAlpha(0.3f);
        extendedSinLine01.LineWidth = 1;

        var x_01 = data01.Select(p => p.t).ToArray();
        var sin_exact_01 = data01.Select(p => p.sin_exact).ToArray();
        var sin_approx_01 = data01.Select(p => p.sin_approx).ToArray();
        var sin_improved_01 = data01.Select(p => p.sin_improved).ToArray();

        var exactLine01 = plt_sin_01.Add.Scatter(x_01, sin_exact_01);
        exactLine01.Color = Colors.Blue;
        exactLine01.LineWidth = 3;
        exactLine01.Label = "Точное значение";

        var approxLine01 = plt_sin_01.Add.Scatter(x_01, sin_approx_01);
        approxLine01.Color = Colors.Red;
        approxLine01.LineWidth = 2;
        approxLine01.Label = $"Ряд Маклорена (n={n_sin_01})";

        var improvedLine01 = plt_sin_01.Add.Scatter(x_01, sin_improved_01);
        improvedLine01.Color = Colors.Green;
        improvedLine01.LineWidth = 2;
        improvedLine01.Label = "Улучшенный алгоритм";

        plt_sin_01.Title($"sin(t) на отрезке [0,1]");
        plt_sin_01.XLabel("t");
        plt_sin_01.YLabel("sin(t)");
        plt_sin_01.ShowLegend();
        plt_sin_01.SavePng("sin_01_extended.png", 800, 600);

        Console.WriteLine("sin_01_extended.png created!");

        var plt_sin_1011 = new Plot();
        var extended_sin_x_1011 = new List<double>();
        var extended_sin_y_1011 = new List<double>();
        for (double t = 9.0; t <= 12.0; t += 0.02) {
            extended_sin_x_1011.Add(t);
            extended_sin_y_1011.Add(Math.Sin(t));
        }
        var extendedSinLine1011 = plt_sin_1011.Add.Scatter(extended_sin_x_1011.ToArray(), extended_sin_y_1011.ToArray());
        extendedSinLine1011.Color = Colors.Gray.WithAlpha(0.3f);
        extendedSinLine1011.LineWidth = 1;

        var x_1011 = data1011.Select(p => p.t).ToArray();
        var sin_exact_1011 = data1011.Select(p => p.sin_exact).ToArray();
        var sin_approx_1011 = data1011.Select(p => p.sin_approx).ToArray();
        var sin_improved_1011 = data1011.Select(p => p.sin_improved).ToArray();

        var exactLine1011 = plt_sin_1011.Add.Scatter(x_1011, sin_exact_1011);
        exactLine1011.Color = Colors.Blue;
        exactLine1011.LineWidth = 3;
        exactLine1011.Label = "Точное значение";

        var approxLine1011 = plt_sin_1011.Add.Scatter(x_1011, sin_approx_1011);
        approxLine1011.Color = Colors.Red;
        approxLine1011.LineWidth = 2;
        approxLine1011.Label = $"Ряд Маклорена (n={n_sin_1011})";

        var improvedLine1011 = plt_sin_1011.Add.Scatter(x_1011, sin_improved_1011);
        improvedLine1011.Color = Colors.Green;
        improvedLine1011.LineWidth = 2;
        improvedLine1011.Label = "Улучшенный алгоритм";

        plt_sin_1011.Title($"sin(t) на отрезке [10,11]");
        plt_sin_1011.XLabel("t");
        plt_sin_1011.YLabel("sin(t)");
        plt_sin_1011.ShowLegend();
        plt_sin_1011.SavePng("sin_1011_extended.png", 800, 600);

        Console.WriteLine("sin_1011_extended.png created!");

        var plt_exp_1011 = new Plot();
        var extended_exp_x_1011 = new List<double>();
        var extended_exp_y_1011 = new List<double>();
        for (double t = 9.0; t <= 12.0; t += 0.02) {
            extended_exp_x_1011.Add(t);
            extended_exp_y_1011.Add(Math.Exp(t));
        }
        var extendedExpLine1011 = plt_exp_1011.Add.Scatter(extended_exp_x_1011.ToArray(), extended_exp_y_1011.ToArray());
        extendedExpLine1011.Color = Colors.Gray.WithAlpha(0.3f);
        extendedExpLine1011.LineWidth = 1;

        var exp_exact_1011 = data1011.Select(p => p.exp_exact).ToArray();
        var exp_approx_1011 = data1011.Select(p => p.exp_approx).ToArray();
        var exp_improved_1011 = data1011.Select(p => p.exp_improved).ToArray();

        var expExactLine1011 = plt_exp_1011.Add.Scatter(x_1011, exp_exact_1011);
        expExactLine1011.Color = Colors.Blue;
        expExactLine1011.LineWidth = 3;
        expExactLine1011.Label = "Точное значение";

        var expApproxLine1011 = plt_exp_1011.Add.Scatter(x_1011, exp_approx_1011);
        expApproxLine1011.Color = Colors.Red;
        expApproxLine1011.LineWidth = 2;
        expApproxLine1011.Label = $"Ряд Маклорена (n={n_exp_1011})";

        var expImprovedLine1011 = plt_exp_1011.Add.Scatter(x_1011, exp_improved_1011);
        expImprovedLine1011.Color = Colors.Green;
        expImprovedLine1011.LineWidth = 2;
        expImprovedLine1011.Label = "Улучшенный алгоритм";

        plt_exp_1011.Title($"exp(t) на отрезке [10,11]");
        plt_exp_1011.XLabel("t");
        plt_exp_1011.YLabel("exp(t)");
        plt_exp_1011.ShowLegend();
        plt_exp_1011.SavePng("exp_1011_extended.png", 800, 600);

        Console.WriteLine("exp_1011_extended.png created!");

        var plt_errors_1011 = new Plot();
        var sin_errors_1011 = new double[data1011.Count];
        var exp_errors_1011 = new double[data1011.Count];

        for (int i = 0; i < data1011.Count; i++) {
            sin_errors_1011[i] = Math.Abs(data1011[i].sin_exact - data1011[i].sin_approx);
            exp_errors_1011[i] = Math.Abs(data1011[i].exp_exact - data1011[i].exp_approx);
        }

        var sinErrorLine1011 = plt_errors_1011.Add.Scatter(x_1011, sin_errors_1011);
        sinErrorLine1011.Color = Colors.Red;
        sinErrorLine1011.LineWidth = 2;
        sinErrorLine1011.Label = "Погрешность sin(t)";

        var expErrorLine1011 = plt_errors_1011.Add.Scatter(x_1011, exp_errors_1011);
        expErrorLine1011.Color = Colors.Blue;
        expErrorLine1011.LineWidth = 2;
        expErrorLine1011.Label = "Погрешность exp(t)";

        var targetLine1011 = plt_errors_1011.Add.HorizontalLine(0.001);
        targetLine1011.Color = Colors.Black;
        targetLine1011.LineWidth = 2;

        plt_errors_1011.Title($"Погрешности на отрезке [10,11]");
        plt_errors_1011.XLabel("t");
        plt_errors_1011.YLabel("Абсолютная погрешность");
        plt_errors_1011.ShowLegend();
        plt_errors_1011.SavePng("errors_1011.png", 800, 600);

        Console.WriteLine("errors_1011.png created!");

        Console.WriteLine($"\nСтатистика для [0,1]:");
        Console.WriteLine($"Макс. погрешность sin: {sin_errors_1011.Max():E6}");
        Console.WriteLine($"Макс. погрешность exp: {exp_errors_1011.Max():E6}");
        Console.WriteLine($"Целевая погрешность: 0.001");

        Console.WriteLine($"\nСтатистика для [10,11]:");
        Console.WriteLine($"Макс. погрешность sin: {sin_errors_1011.Max():E6}");
        Console.WriteLine($"Макс. погрешность exp: {exp_errors_1011.Max():E6}");
        Console.WriteLine($"Целевая погрешность: 0.001");
    }
}

public class DataPoint {
    public double t { get; set; }
    public double sin_exact { get; set; }
    public double sin_approx { get; set; }
    public double exp_exact { get; set; }
    public double exp_approx { get; set; }
    public double sin_improved { get; set; }
    public double exp_improved { get; set; }
}