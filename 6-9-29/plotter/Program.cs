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
            var (originalData, newtonData, splineData, actualPoint, results) = ParseDataFile(lines);
            CreatePopulationPlot(originalData, newtonData, splineData, actualPoint, results);
        }
        catch (Exception ex) {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    static (List<DataPoint> originalData, List<DataPoint> newtonData, List<DataPoint> splineData,
            DataPoint actualPoint, PlotResults results) ParseDataFile(string[] lines) {
        var originalData = new List<DataPoint>();
        var newtonData = new List<DataPoint>();
        var splineData = new List<DataPoint>();
        var actualPoint = new DataPoint();
        var results = new PlotResults();

        bool readingGraphData = false;
        string currentSection = "";

        foreach (var line in lines) {
            if (line.Contains("newton_2010 =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    results.Newton2010 = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("spline_2010 =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    results.Spline2010 = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("actual_2010 =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    results.Actual2010 = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("newton_error =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    results.NewtonError = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("spline_error =")) {
                var parts = line.Split('=');
                if (parts.Length == 2)
                    results.SplineError = double.Parse(parts[1].Trim(), CultureInfo.InvariantCulture);
            }
            else if (line.Contains("ДАННЫЕ ДЛЯ ГРАФИКА:")) {
                readingGraphData = true;
                continue;
            }
            else if (line.StartsWith("Год\tНаселение\tТип") || string.IsNullOrWhiteSpace(line))
                continue;

            if (readingGraphData) {
                if (line == "исходные") {
                    currentSection = "original";
                    continue;
                }
                else if (line == "точное") {
                    currentSection = "actual";
                    continue;
                }
                else if (line == "ньютон") {
                    currentSection = "newton";
                    continue;
                }
                else if (line == "сплайн") {
                    currentSection = "spline";
                    continue;
                }

                var dataParts = line.Split('\t');
                if (dataParts.Length >= 2) {
                    var point = new DataPoint {
                        Year = int.Parse(dataParts[0]),
                        Population = double.Parse(dataParts[1], CultureInfo.InvariantCulture),
                        Type = currentSection
                    };

                    switch (currentSection) {
                        case "original":
                            originalData.Add(point);
                            break;
                        case "actual":
                            actualPoint = point;
                            break;
                        case "newton":
                            newtonData.Add(point);
                            break;
                        case "spline":
                            splineData.Add(point);
                            break;
                    }
                }
            }
        }
        return (originalData, newtonData, splineData, actualPoint, results);
    }

    static void CreatePopulationPlot(List<DataPoint> originalData, List<DataPoint> newtonData,
                                   List<DataPoint> splineData, DataPoint actualPoint, PlotResults results) {
        var plt = new Plot();
        // Сортируем данные по году
        newtonData = newtonData.OrderBy(p => p.Year).ToList();
        splineData = splineData.OrderBy(p => p.Year).ToList();

        var originalYears = originalData.Select(p => (double)p.Year).ToArray();
        var originalPopulation = originalData.Select(p => p.Population).ToArray();
        var originalScatter = plt.Add.Scatter(originalYears, originalPopulation);
        originalScatter.Color = Colors.Black;
        originalScatter.MarkerSize = 8;
        originalScatter.MarkerShape = MarkerShape.FilledCircle;
        originalScatter.LineWidth = 0;
        originalScatter.Label = "Исходные данные";

        var newtonYears = newtonData.Select(p => (double)p.Year).ToArray();
        var newtonPopulation = newtonData.Select(p => p.Population).ToArray();
        var newtonLine = plt.Add.Scatter(newtonYears, newtonPopulation);
        newtonLine.Color = Colors.Red;
        newtonLine.LineWidth = 2;
        newtonLine.Label = $"Полином Ньютона";

        var splineYears = splineData.Select(p => (double)p.Year).ToArray();
        var splinePopulation = splineData.Select(p => p.Population).ToArray();
        var splineLine = plt.Add.Scatter(splineYears, splinePopulation);
        splineLine.Color = Colors.Blue;
        splineLine.LineWidth = 2;
        splineLine.Label = $"Линейный сплайн";

        var actualPointX = new double[] { actualPoint.Year };
        var actualPointY = new double[] { actualPoint.Population };
        var actualScatter = plt.Add.Scatter(actualPointX, actualPointY);
        actualScatter.Color = Colors.Green;
        actualScatter.MarkerSize = 10;
        actualScatter.MarkerShape = MarkerShape.FilledCircle;
        actualScatter.Label = $"Точное значение 2010: {results.Actual2010 / 1e6:F1} млн";

        var newtonPredictionX = new double[] { 2010 };
        var newtonPredictionY = new double[] { results.Newton2010 };
        var newtonPrediction = plt.Add.Scatter(newtonPredictionX, newtonPredictionY);
        newtonPrediction.Color = Colors.DarkRed;
        newtonPrediction.MarkerSize = 8;
        newtonPrediction.MarkerShape = MarkerShape.FilledCircle;

        var splinePredictionX = new double[] { 2010 };
        var splinePredictionY = new double[] { results.Spline2010 };
        var splinePrediction = plt.Add.Scatter(splinePredictionX, splinePredictionY);
        splinePrediction.Color = Colors.DarkBlue;
        splinePrediction.MarkerSize = 8;
        splinePrediction.MarkerShape = MarkerShape.FilledCircle;

        plt.Title("Интерполяция и экстраполяция численности населения США", size: 16);
        plt.XLabel("Год");
        plt.YLabel("Население, человек");

        plt.ShowLegend(Alignment.UpperLeft);

        double xMin = 1900;
        double xMax = 2020;
        double yMin = 0;
        double yMax = Math.Max(Math.Max(results.Newton2010, results.Spline2010), results.Actual2010) * 1.1;
        plt.Axes.SetLimits(xMin, xMax, yMin, yMax);

        var vline = plt.Add.VerticalLine(2010);
        vline.Color = Colors.Green;
        vline.LineWidth = 1;

        plt.SavePng("population_extrapolation.png", 1400, 900);
        Console.WriteLine("График сохранен как: population_extrapolation.png");
    }
}

public class DataPoint {
    public int Year { get; set; }
    public double Population { get; set; }
    public string Type { get; set; }
}

public class PlotResults {
    public double Newton2010 { get; set; }
    public double Spline2010 { get; set; }
    public double Actual2010 { get; set; }
    public double NewtonError { get; set; }
    public double SplineError { get; set; }
}