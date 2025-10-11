using System;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Collections.Generic;
using ScottPlot;

class Program {
    static void Main(string[] args) {
        string dataFile = "../data_generator/data/fwhm_results.txt";
        if (!File.Exists(dataFile)) {
            Console.WriteLine("Файл с данными не найден!");
            Console.WriteLine("Сначала запустите C++ программу для генерации данных.");
            return;
        }

        try {
            var lines = File.ReadAllLines(dataFile);
            var (functionData, parameters) = ParseDataFile(lines);

            CreateFWHMPlot(functionData, parameters);
        }
        catch (Exception ex) {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    static (List<DataPoint> functionData, PlotParameters parameters) ParseDataFile(string[] lines) {
        var functionData = new List<DataPoint>();
        var parameters = new PlotParameters();

        bool readingData = false;

        foreach (var line in lines) {
            if (line.Contains("x_max =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    parameters.XMax = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("f_max =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    parameters.FMax = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("Полувысота:")) {
                var parts = line.Split(':');
                if (parts.Length == 2)
                    parameters.Target = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("Левая точка: x_left =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    parameters.XLeft = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("Правая точка: x_right =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    parameters.XRight = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("FWHM =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    parameters.FWHM = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("ДАННЫЕ ДЛЯ ГРАФИКА:")) {
                readingData = true;
                continue;
            }
            else if (line.StartsWith("x\tf(x)") || string.IsNullOrWhiteSpace(line))
                continue;

            if (readingData) {
                var dataParts = line.Split('\t');
                if (dataParts.Length >= 2) {
                    var point = new DataPoint {
                        X = double.Parse(dataParts[0], CultureInfo.InvariantCulture),
                        Y = double.Parse(dataParts[1], CultureInfo.InvariantCulture)
                    };
                    functionData.Add(point);
                }
            }
        }
        return (functionData, parameters);
    }

    static void CreateFWHMPlot(List<DataPoint> functionData, PlotParameters parameters) {
        var plt = new Plot();

        var xValues = functionData.Select(p => p.X).ToArray();
        var yValues = functionData.Select(p => p.Y).ToArray();

        var functionLine = plt.Add.Scatter(xValues, yValues);
        functionLine.Color = Colors.Blue;
        functionLine.LineWidth = 3;
        functionLine.Label = "f(x) = x⋅exp(-x^2)";

        var targetLineX = new double[] {0, 2};
        var targetLineY = new double[] {parameters.Target, parameters.Target};
        var targetLine = plt.Add.Scatter(targetLineX, targetLineY);
        targetLine.Color = Colors.Red;
        targetLine.LineWidth = 2;
        targetLine.Label = $"Полувысота = {parameters.Target:F4}";

        var fwhmPointsX = new double[] {parameters.XLeft, parameters.XRight};
        var fwhmPointsY = new double[] {parameters.Target, parameters.Target};
        var fwhmScatter = plt.Add.Scatter(fwhmPointsX, fwhmPointsY);
        fwhmScatter.Color = Colors.Green;
        fwhmScatter.MarkerSize = 12;
        fwhmScatter.MarkerShape = MarkerShape.FilledCircle;
        fwhmScatter.Label = "Точки полувысоты";

        var maxPointX = new double[] {parameters.XMax};
        var maxPointY = new double[] {parameters.FMax};
        var maxScatter = plt.Add.Scatter(maxPointX, maxPointY);
        maxScatter.Color = Colors.Orange;
        maxScatter.MarkerSize = 12;
        maxScatter.MarkerShape = MarkerShape.FilledCircle;
        maxScatter.Label = "Максимум";

        plt.Title("Ширина функции на полувысоте (FWHM)", size: 16);
        plt.XLabel("x");
        plt.YLabel("f(x)");
        plt.ShowLegend(Alignment.UpperRight);
        plt.Axes.SetLimits(0, 1.5, 0, parameters.FMax * 1.2);

        plt.SavePng("FWHM.png", 1000, 600);
    }
}

public class DataPoint
{
    public double X {get; set;}
    public double Y {get; set;}
}

public class PlotParameters {
    public double XMax { get; set; }
    public double FMax { get; set; }
    public double Target { get; set; }
    public double XLeft { get; set; }
    public double XRight { get; set; }
    public double FWHM { get; set; }
}