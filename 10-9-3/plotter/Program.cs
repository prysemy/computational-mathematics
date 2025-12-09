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
        Console.WriteLine("=== Rayleigh Equation Visualization ===");

        string dataFile = "../data_generator/data/results.csv";

        if (!File.Exists(dataFile))
        {
            Console.WriteLine($"File {dataFile} not found!");
            Console.WriteLine("Run C++ program first to generate data.");
            return;
        }

        try
        {
            var dataPoints = ParseDataFile(dataFile);
            Console.WriteLine($"Read {dataPoints.Count} points from {dataFile}");

            CreatePositionPlot(dataPoints);
            CreateVelocityPlot(dataPoints);
            CreatePhasePortrait(dataPoints);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
    }

    public class DataPoint
    {
        public double t { get; set; }
        public double x { get; set; }
        public double dx { get; set; }
    }

    static List<DataPoint> ParseDataFile(string filename)
    {
        var points = new List<DataPoint>();

        var lines = File.ReadAllLines(filename);

        foreach (var line in lines.Skip(1))
        {
            if (string.IsNullOrWhiteSpace(line))
                continue;

            var parts = line.Split(',');
            if (parts.Length >= 3)
            {
                var point = new DataPoint
                {
                    t = double.Parse(parts[0], CultureInfo.InvariantCulture),
                    x = double.Parse(parts[1], CultureInfo.InvariantCulture),
                    dx = double.Parse(parts[2], CultureInfo.InvariantCulture)
                };
                points.Add(point);
            }
        }

        return points;
    }

    static void CreatePositionPlot(List<DataPoint> points)
    {
        var plt = new Plot();
        plt.XLabel("Time (t)");
        plt.YLabel("Position (x(t))");

        var t = points.Select(p => p.t).ToArray();
        var x = points.Select(p => p.x).ToArray();

        var positionLine = plt.Add.Scatter(t, x);
        positionLine.Color = ScottPlot.Color.FromHex("#1f77b4");
        positionLine.LineWidth = 2;

        // Add horizontal line at x=0
        var zeroLine = plt.Add.HorizontalLine(0);
        zeroLine.Color = ScottPlot.Color.FromHex("#7f7f7f").WithAlpha(0.3f);
        zeroLine.LineWidth = 1;

        // Highlight initial point
        if (points.Count > 0)
        {
            var initPoint = plt.Add.Marker(points[0].t, points[0].x,
                                          ScottPlot.MarkerShape.FilledCircle, 10,
                                          ScottPlot.Color.FromHex("#ff7f0e"));
        }

        string filename = "rayleigh_position.png";
        plt.SavePng(filename, 1200, 600);
        Console.WriteLine($"Created: {filename}");
    }

    static void CreateVelocityPlot(List<DataPoint> points)
    {
        var plt = new Plot();
        plt.XLabel("Time (t)");
        plt.YLabel("Velocity (dx/dt)");

        var t = points.Select(p => p.t).ToArray();
        var dx = points.Select(p => p.dx).ToArray();

        var velocityLine = plt.Add.Scatter(t, dx);
        velocityLine.Color = ScottPlot.Color.FromHex("#d62728");
        velocityLine.LineWidth = 2;

        // Add horizontal lines at ±1 (velocity limit for Rayleigh equation)
        var plusOneLine = plt.Add.HorizontalLine(1);
        plusOneLine.Color = ScottPlot.Color.FromHex("#2ca02c").WithAlpha(0.3f);
        plusOneLine.LineWidth = 1;

        var minusOneLine = plt.Add.HorizontalLine(-1);
        minusOneLine.Color = ScottPlot.Color.FromHex("#2ca02c").WithAlpha(0.3f);
        minusOneLine.LineWidth = 1;

        // Highlight initial point
        if (points.Count > 0)
        {
            var initPoint = plt.Add.Marker(points[0].t, points[0].dx,
                                          ScottPlot.MarkerShape.FilledCircle, 10,
                                          ScottPlot.Color.FromHex("#ff7f0e"));
        }

        string filename = "rayleigh_velocity.png";
        plt.SavePng(filename, 1200, 600);
        Console.WriteLine($"Created: {filename}");
    }

    static void CreatePhasePortrait(List<DataPoint> points)
    {
        var plt = new Plot();
        plt.XLabel("Position (x)");
        plt.YLabel("Velocity (dx/dt)");

        var x = points.Select(p => p.x).ToArray();
        var dx = points.Select(p => p.dx).ToArray();

        var phaseLine = plt.Add.Scatter(x, dx);
        phaseLine.Color = ScottPlot.Color.FromHex("#2ca02c");
        phaseLine.LineWidth = 1;

        // Highlight initial point
        if (points.Count > 0)
        {
            var initPoint = plt.Add.Marker(points[0].x, points[0].dx,
                                          ScottPlot.MarkerShape.FilledCircle, 15,
                                          ScottPlot.Color.FromHex("#ff7f0e"));
        }

        // Add ellipse showing theoretical limit cycle (for Rayleigh equation)
        // Rayleigh equation tends to limit cycle near x² + (dx/dt)² ≈ constant
        var limitCycle = plt.Add.Ellipse(0, 0, 2.0, 2.0);
        limitCycle.LineWidth = 2;

        string filename = "rayleigh_phase.png";
        plt.SavePng(filename, 800, 800);
        Console.WriteLine($"Created: {filename}");
    }

}
