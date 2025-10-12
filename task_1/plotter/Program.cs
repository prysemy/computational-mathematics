using System;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Collections.Generic;
using ScottPlot;

class Program {
    static void Main(string[] args) {
        string dataFile = "../data_generator/data/results.txt";
        if (!File.Exists(dataFile)) {
            Console.WriteLine("Файл с данными не найден!");
            Console.WriteLine("Сначала запустите C++ программу для генерации данных.");
            return;
        }

        try {
            string[] lines = File.ReadAllLines(dataFile);
            var (graphData, roots) = ParseDataFile(lines);
            CreateEquationPlot(graphData, roots);
        }
        catch (Exception ex) {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    static (List<GraphPoint> graphData, List<RootPoint> roots) ParseDataFile(string[] lines) {
        var graphData = new List<GraphPoint>();
        var roots = new List<RootPoint>();

        bool readingGraphData = false;
        bool readingRoots = false;

        foreach (var line in lines) {
            if (string.IsNullOrWhiteSpace(line))
                continue;

            if (line.Contains("ДАННЫЕ ДЛЯ ГРАФИКА:")) {
                readingGraphData = true;
                readingRoots = false;
                continue;
            }
            else if (line.Contains("ВСЕ НАЙДЕННЫЕ КОРНИ:")) {
                readingGraphData = false;
                readingRoots = true;
                continue;
            }
            else if (line.StartsWith("x\tcircle_upper\tcircle_lower\ttan") || line.StartsWith("Корень:"))
                continue;

            if (readingGraphData) {
                var parts = line.Split('\t');
                if (parts.Length >= 4) {
                    var point = new GraphPoint {
                        X = double.Parse(parts[0], CultureInfo.InvariantCulture),
                        CircleUpper = ParseDoubleOrNaN(parts[1]),
                        CircleLower = ParseDoubleOrNaN(parts[2]),
                        Tangent = ParseDoubleOrNaN(parts[3])
                    };
                    graphData.Add(point);
                }
            }
            else if (readingRoots && line.StartsWith("Корень ")) {
                var start = line.IndexOf('(') + 1;
                var end = line.IndexOf(')');
                if (start > 0 && end > start) {
                    var coords = line.Substring(start, end - start).Split(',');
                    if (coords.Length == 2) {
                        var root = new RootPoint {
                            X = double.Parse(coords[0].Trim(), CultureInfo.InvariantCulture),
                            Y = double.Parse(coords[1].Trim(), CultureInfo.InvariantCulture),
                            Index = roots.Count + 1
                        };
                        roots.Add(root);
                    }
                }
            }
        }
        return (graphData, roots);
    }

    static double ParseDoubleOrNaN(string value) {
        if (string.IsNullOrWhiteSpace(value) || value.ToLower() == "nan")
            return double.NaN;
        return double.Parse(value, CultureInfo.InvariantCulture);
    }

    static void CreateEquationPlot(List<GraphPoint> graphData, List<RootPoint> roots) {
        var plt = new Plot();

        var validCircleUpper = graphData.Where(p => !double.IsNaN(p.CircleUpper)).ToList();
        var validCircleLower = graphData.Where(p => !double.IsNaN(p.CircleLower)).ToList();
        var validTangent = graphData.Where(p => !double.IsNaN(p.Tangent)).ToList();

        var circleUpperX = validCircleUpper.Select(p => p.X).ToArray();
        var circleUpperY = validCircleUpper.Select(p => p.CircleUpper).ToArray();
        var circleUpperLine = plt.Add.Scatter(circleUpperX, circleUpperY);
        circleUpperLine.Color = Colors.Blue;
        circleUpperLine.LineWidth = 2;
        var circleLowerX = validCircleLower.Select(p => p.X).ToArray();
        var circleLowerY = validCircleLower.Select(p => p.CircleLower).ToArray();
        var circleLowerLine = plt.Add.Scatter(circleLowerX, circleLowerY);
        circleLowerLine.Color = Colors.Blue;
        circleLowerLine.LineWidth = 2;
        circleLowerLine.Label = "x^2 + y^2 = 1";

        var tangentX = validTangent.Select(p => p.X).ToArray();
        var tangentY = validTangent.Select(p => p.Tangent).ToArray();
        var tangentLine = plt.Add.Scatter(tangentX, tangentY);
        tangentLine.Color = Colors.Red;
        tangentLine.LineWidth = 2;
        tangentLine.Label = "y = tg(x)";

        var rootsX = roots.Select(r => r.X).ToArray();
        var rootsY = roots.Select(r => r.Y).ToArray();
        var rootsScatter = plt.Add.ScatterPoints(rootsX, rootsY);
        rootsScatter.Color = Colors.Green;
        rootsScatter.MarkerSize = 17;
        rootsScatter.MarkerShape = MarkerShape.FilledCircle;
        rootsScatter.Label = $"Корни системы ({roots.Count} шт.)";

        plt.Title("Система уравнений: x^2 + y^2 = 1 и y = tg(x)", size: 16);
        plt.XLabel("x");
        plt.YLabel("y");

        plt.ShowLegend(Alignment.UpperRight);
        plt.Axes.SetLimits(-2, 2, -1.5, 1.5);

        plt.SavePng("equation_system.png", 1200, 800);
    }
}

public class GraphPoint {
    public double X { get; set; }
    public double CircleUpper { get; set; }
    public double CircleLower { get; set; }
    public double Tangent { get; set; }
}

public class RootPoint {
    public double X { get; set; }
    public double Y { get; set; }
    public int Index { get; set; }
}