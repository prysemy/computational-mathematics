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
        Console.WriteLine("=== ГРАФИК РЕШЕНИЯ КРАЕВОЙ ЗАДАЧИ ===");
        Console.WriteLine("y'' + (x²-3)y' + (x²-3)cos(x)y = f(x)");
        Console.WriteLine("y(0) = 0, y(π) = π²\n");

        string dataFile = "../data_generator/data/results.csv";

        if (!File.Exists(dataFile))
        {
            Console.WriteLine($"Файл {dataFile} не найден!");
            Console.WriteLine("Сначала запустите C++ программу (main.cpp) для генерации данных.");
            return;
        }

        try
        {
            var dataPoints = ParseDataFile(dataFile);
            Console.WriteLine($"Загружено {dataPoints.Count} точек решения.\n");

            // Точки из условия задачи
            var targetPoints = new[] { 0.5, 1.0, 1.5, 2.0, 2.5, 3.0 };

            // 1. Выводим значения в консоль
            Console.WriteLine("===========================================");
            Console.WriteLine("ОТВЕТ: ЗНАЧЕНИЯ ФУНКЦИИ В ТОЧКАХ");
            Console.WriteLine("===========================================");
            Console.WriteLine("x\t\ty(x)");
            Console.WriteLine("-------------------------------------------");

            foreach (double targetX in targetPoints)
            {
                double yValue = Interpolate(dataPoints, targetX);
                Console.WriteLine($"{targetX:F1}\t\t{yValue:E12}");
            }

            // 2. Строим графики
            CreatePlots(dataPoints, targetPoints);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    public class DataPoint
    {
        public double x { get; set; }
        public double y { get; set; }
    }

    static List<DataPoint> ParseDataFile(string filename)
    {
        var points = new List<DataPoint>();

        var lines = File.ReadAllLines(filename);

        foreach (var line in lines.Skip(1)) // Пропускаем заголовок
        {
            if (string.IsNullOrWhiteSpace(line))
                continue;

            var parts = line.Split(',');
            if (parts.Length >= 2)
            {
                var point = new DataPoint
                {
                    x = double.Parse(parts[0], CultureInfo.InvariantCulture),
                    y = double.Parse(parts[1], CultureInfo.InvariantCulture)
                };
                points.Add(point);
            }
        }

        return points.OrderBy(p => p.x).ToList();
    }

    static double Interpolate(List<DataPoint> points, double targetX)
    {
        if (points.Count == 0) return 0;
        if (targetX <= points.First().x) return points.First().y;
        if (targetX >= points.Last().x) return points.Last().y;

        for (int i = 0; i < points.Count - 1; i++)
        {
            if (points[i].x <= targetX && points[i + 1].x >= targetX)
            {
                double x1 = points[i].x;
                double x2 = points[i + 1].x;
                double y1 = points[i].y;
                double y2 = points[i + 1].y;

                double t = (targetX - x1) / (x2 - x1);
                return y1 + t * (y2 - y1);
            }
        }

        return points.Last().y;
    }

    static void CreatePlots(List<DataPoint> points, double[] targetPoints)
    {
        // Преобразуем данные в массивы для ScottPlot
        double[] xData = points.Select(p => p.x).ToArray();
        double[] yData = points.Select(p => p.y).ToArray();

        // 1. ОСНОВНОЙ ГРАФИК РЕШЕНИЯ
        var plt1 = new Plot();
        plt1.Title("Решение краевой задачи");
        plt1.XLabel("x");
        plt1.YLabel("y(x)");

        // Линия решения
        var line = plt1.Add.Scatter(xData, yData);
        line.LineWidth = 2;
        line.Color = Colors.Blue;

        // Горизонтальная линия y=0
        var zeroLine = plt1.Add.HorizontalLine(0);
        zeroLine.LineWidth = 1;
        zeroLine.Color = Colors.Gray.WithAlpha(0.3);

        // Вертикальные линии для граничных точек
        var leftBoundary = plt1.Add.VerticalLine(0);
        leftBoundary.LineWidth = 1;
        leftBoundary.Color = Colors.Red.WithAlpha(0.5);

        var rightBoundary = plt1.Add.VerticalLine(Math.PI);
        rightBoundary.LineWidth = 1;
        rightBoundary.Color = Colors.Red.WithAlpha(0.5);

        // Точки граничных условий
        var boundaryPoints = plt1.Add.Markers(
            new double[] { 0, Math.PI },
            new double[] { points.First().y, points.Last().y }
        );
        boundaryPoints.Color = Colors.Red;

        plt1.SavePng("solution_plot.png", 1200, 600);

    }
}