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
        string dataFile = "../data_generator/data/results.txt";
        if (!File.Exists(dataFile))
        {
            Console.WriteLine("Файл с данными не найден!");
            Console.WriteLine("Сначала запустите C++ программу для генерации данных.");
            return;
        }

        try
        {
            string[] allLines = File.ReadAllLines(dataFile);
            ProcessDataFile(allLines);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    static void ProcessDataFile(string[] lines) {
        // Читаем данные функции
        var functionData = ReadFunctionData(lines);
        if (functionData.Count == 0) {
            Console.WriteLine("Не удалось прочитать данные функции");
            return;
        }

        // Читаем результаты методов
        var methodResults = ReadMethodResults(lines);
        if (methodResults.Count == 0) {
            Console.WriteLine("Не удалось прочитать результаты методов");
            return;
        }

        // Создаем графики
        CreateFunctionPlot(functionData);
        CreateDetailedMethodPlots(functionData, methodResults);
        PrintResults(methodResults);
    }


    // Создание тестовых данных функции
    static List<FunctionPoint> CreateTestFunctionData()
    {
        var data = new List<FunctionPoint>();
        for (double x = 0; x <= 3.0; x += 0.01)
        {
            double y = Math.Sin(100 * x) * Math.Exp(-x * x) * Math.Cos(2 * x);
            data.Add(new FunctionPoint { X = x, Y = y });
        }
        return data;
    }

    // Создание тестовых результатов методов
    static List<MethodResult> CreateTestMethodResults()
    {
        return new List<MethodResult>
        {
            new MethodResult { MethodName = "Метод прямоугольников", Value = 1.234e-5 },
            new MethodResult { MethodName = "Метод трапеций", Value = 1.235e-5 },
            new MethodResult { MethodName = "Метод Симпсона", Value = 1.233e-5 },
            new MethodResult { MethodName = "Правило 3/8", Value = 1.234e-5 },
            new MethodResult { MethodName = "Гаусс 2 узла", Value = 1.232e-5 },
            new MethodResult { MethodName = "Гаусс 3 узла", Value = 1.233e-5 },
            new MethodResult { MethodName = "Гаусс 4 узла", Value = 1.233e-5 }
        };
    }

    static List<FunctionPoint> ReadFunctionData(string[] lines)
    {
        var data = new List<FunctionPoint>();
        bool readingFunctionData = false;

        for (int i = 0; i < lines.Length; i++)
        {
            var line = lines[i].Trim();

            // Ищем начало блока с данными функции
            if (line.Contains("ДАННЫЕ ДЛЯ ГРАФИКА ФУНКЦИИ:") ||
                line.Contains("x\t\tf(x)") ||
                line.Contains("x               f(x)"))
            {
                readingFunctionData = true;
                continue;
            }

            if (readingFunctionData)
            {
                // Пропускаем заголовки и пустые строки
                if (string.IsNullOrWhiteSpace(line) ||
                    line.Contains("x") && line.Contains("f(x)"))
                    continue;

                // Парсим строку с данными
                var parts = line.Split(new[] { '\t', ' ' }, StringSplitOptions.RemoveEmptyEntries);

                if (parts.Length >= 2)
                {
                    // Пробуем разные форматы чисел
                    if (double.TryParse(parts[0], NumberStyles.Any, CultureInfo.InvariantCulture, out double x) &&
                        double.TryParse(parts[1], NumberStyles.Any, CultureInfo.InvariantCulture, out double y))
                    {
                        data.Add(new FunctionPoint { X = x, Y = y });
                    }
                    else
                    {
                        // Дополнительная попытка парсинга с учетом научной нотации
                        if (TryParseScientific(parts[0], out x) && TryParseScientific(parts[1], out y))
                        {
                            data.Add(new FunctionPoint { X = x, Y = y });
                        }
                        else
                        {
                            Console.WriteLine($"Не удалось распарсить строку: {line}");
                        }
                    }
                }

                // Ограничиваем количество точек для производительности
                if (data.Count >= 1000)
                    break;
            }
        }

        // Если данных мало, создаем тестовые
        if (data.Count < 10)
        {
            Console.WriteLine("Созданы тестовые данные функции");
            return CreateTestFunctionData();
        }

        return data;
    }

    // Вспомогательная функция для парсинга научной нотации
    static bool TryParseScientific(string str, out double result)
    {
        // Заменяем 'e' на 'E' для стандартного парсинга
        str = str.Replace('e', 'E').Replace('Е', 'E'); // кириллическая E на латинскую

        return double.TryParse(str, NumberStyles.Any, CultureInfo.InvariantCulture, out result);
    }

    static List<MethodResult> ReadMethodResults(string[] lines)
    {
        var results = new List<MethodResult>();
        bool readingMethods = false;

        foreach (var line in lines)
        {
            if (line.Contains("СРАВНЕНИЕ МЕТОДОВ"))
            {
                readingMethods = true;
                continue;
            }

            if (readingMethods && line.Contains(':'))
            {
                var parts = line.Split(':');
                if (parts.Length == 2)
                {
                    var name = parts[0].Trim();
                    var valueStr = parts[1].Trim();

                    if (double.TryParse(valueStr, NumberStyles.Any, CultureInfo.InvariantCulture, out double value))
                    {
                        results.Add(new MethodResult { MethodName = name, Value = value });
                    }
                }
            }
        }
        return results;
    }

    static void CreateFunctionPlot(List<FunctionPoint> functionData) {
        var plt = new Plot();

        var xValues = functionData.Select(f => f.X).ToArray();
        var yValues = functionData.Select(f => f.Y).ToArray();

        // График функции
        var functionPlot = plt.Add.Scatter(xValues, yValues);
        functionPlot.Color = Colors.Blue;
        functionPlot.LineWidth = 2;
        functionPlot.LegendText = "f(x) = sin(100x) · exp(-x^2) · cos(2x)";

        // Заливка области под кривой
        var fill = plt.Add.FillY(
                    xs: xValues,
                    ys1: yValues,
                    ys2: new double[xValues.Length] // нулевая линия
                );
        fill.FillColor = Colors.Blue.WithAlpha(.2);

        // Настройки графика
        plt.Title("Подынтегральная функция и область интегрирования", size: 16);
        plt.XLabel("x");
        plt.YLabel("f(x)");
        plt.ShowLegend(Alignment.UpperRight);
        plt.Axes.SetLimits(0, 3, -0.5, 0.5);

        plt.SavePng("function.png", 1200, 600);
    }

    static void CreateDetailedMethodPlots(List<FunctionPoint> functionData, List<MethodResult> methodResults)
    {
        var methodsToVisualize = new List<string>
        {
            "Метод средних прямоугольников",
            "Метод трапеций",
            "Метод Симпсона",
            "Гаусс 2 узла"
        };

        foreach (var methodName in methodsToVisualize)
        {
            var method = methodResults.FirstOrDefault(m => m.MethodName == methodName);
            if (method != null)
            {
                CreateSingleMethodPlot(functionData, method);
            }
        }
    }

    static void CreateSingleMethodPlot(List<FunctionPoint> functionData, MethodResult method)
    {
        var plt = new Plot();
        plt.Title($"{method.MethodName}\nЗначение интеграла: {method.Value:E4}", size: 16);

        var xValues = functionData.Select(f => f.X).ToArray();
        var yValues = functionData.Select(f => f.Y).ToArray();

        // Основной график функции
        var functionPlot = plt.Add.Scatter(xValues, yValues);
        functionPlot.Color = Colors.LightGray;
        functionPlot.LineWidth = 1;
        functionPlot.LegendText = "f(x)";

        // Визуализация метода
        switch (method.MethodName)
        {
            case "Метод средних прямоугольников":
                VisualizeRectangleMethod(plt, xValues, yValues);
                break;
            case "Метод трапеций":
                VisualizeTrapezoidalMethod(plt, xValues, yValues);
                break;
            case "Метод Симпсона":
                VisualizeSimpsonMethod(plt, xValues, yValues);
                break;
            case "Гаусс 2 узла":
                VisualizeGaussMethod(plt, xValues, yValues);
                break;
        }

        plt.XLabel("x");
        plt.YLabel("f(x)");
        plt.ShowLegend(Alignment.UpperRight);
        plt.Axes.SetLimits(0, 3, -0.5, 0.5);

        var filename = $"{method.MethodName.Replace(" ", "_").ToLower()}.png";
        plt.SavePng(filename, 1000, 600);
        Console.WriteLine($"Создан график: {filename}");
    }

    static void VisualizeRectangleMethod(Plot plt, double[] xValues, double[] yValues)
    {
        int n = 20; // Количество интервалов для визуализации
        double a = 0, b = 3;
        double h = (b - a) / n;

        for (int i = 0; i < n; i++)
        {
            double x_mid = a + (i + 0.5) * h; // Средняя точка
            double y_mid = f(x_mid); // Значение в средней точке

            // Прямоугольник
            var rectX = new double[] { a + i * h, a + i * h, a + (i + 1) * h, a + (i + 1) * h };
            var rectY = new double[] { 0, y_mid, y_mid, 0 };

            var polygon = plt.Add.Polygon(rectX, rectY);
            polygon.FillColor = Colors.Red.WithAlpha(.5);
            polygon.LineColor = Colors.Red;
            polygon.LineWidth = 1;

            // Точка в середине
            var point = plt.Add.Marker(x_mid, y_mid);
            point.Color = Colors.Red;
            point.MarkerSize = 8;
            point.MarkerShape = MarkerShape.FilledCircle;
        }
    }

    static void VisualizeTrapezoidalMethod(Plot plt, double[] xValues, double[] yValues)
    {
        int n = 15; // Количество интервалов
        double a = 0, b = 3;
        double h = (b - a) / n;

        for (int i = 0; i < n; i++)
        {
            double x1 = a + i * h;
            double x2 = a + (i + 1) * h;
            double y1 = f(x1);
            double y2 = f(x2);

            // Трапеция
            var trapX = new double[] { x1, x1, x2, x2 };
            var trapY = new double[] { 0, y1, y2, 0 };

            var polygon = plt.Add.Polygon(trapX, trapY);
            polygon.FillColor = Colors.Green.WithAlpha(.5); // Полупрозрачный зеленый
            polygon.LineColor = Colors.Green;
            polygon.LineWidth = 1;

            // Точки на границах
            var point1 = plt.Add.Marker(x1, y1);
            point1.Color = Colors.Green;
            point1.MarkerSize = 6;
            point1.MarkerShape = MarkerShape.FilledDiamond;

            var point2 = plt.Add.Marker(x2, y2);
            point2.Color = Colors.Green;
            point2.MarkerSize = 6;
            point2.MarkerShape = MarkerShape.FilledDiamond;
        }
    }

    static void VisualizeSimpsonMethod(Plot plt, double[] xValues, double[] yValues)
    {
        int n = 12; // Четное количество интервалов
        double a = 0, b = 3;
        double h = (b - a) / n;

        for (int i = 0; i < n; i += 2) // По два интервала за раз
        {
            double x0 = a + i * h;
            double x1 = a + (i + 1) * h;
            double x2 = a + (i + 2) * h;

            double y0 = f(x0);
            double y1 = f(x1);
            double y2 = f(x2);

            // Аппроксимируем параболу
            var parabolaX = new List<double>();
            var parabolaY = new List<double>();

            for (double x = x0; x <= x2; x += h / 4)
            {
                // Квадратичная интерполяция
                double t = (x - x0) / (2 * h);
                double y = y0 * (1 - 3 * t + 2 * t * t) +
                          y1 * (4 * t - 4 * t * t) +
                          y2 * (2 * t * t - t);

                parabolaX.Add(x);
                parabolaY.Add(y);
            }

            // Заливка под параболой
            var fillX = parabolaX.ToArray();
            var fillY = parabolaY.ToArray();
            var zeroY = new double[fillY.Length]; // Нулевая линия

            var fill = plt.Add.FillY(
                    xs: fillX,
                    ys1: fillY,
                    ys2: zeroY
                );
            fill.FillColor = Colors.Blue.WithAlpha(.5);
            fill.LineColor = Colors.Blue;
            fill.LineWidth = 1;

            // Точки
            var pointsX = new double[] { x0, x1, x2 };
            var pointsY = new double[] { y0, y1, y2 };

            var points = plt.Add.Scatter(pointsX, pointsY);
            points.Color = Colors.Blue;
            points.MarkerSize = 6;
            points.MarkerShape = MarkerShape.FilledDiamond;
            points.LineWidth = 0;
        }
    }

    static void VisualizeGaussMethod(Plot plt, double[] xValues, double[] yValues)
    {
        int n = 8; // Количество интервалов
        double a = 0, b = 3;
        double h = (b - a) / n;

        // Узлы Гаусса на [-1, 1]
        double[] gaussNodes = { -1.0 / Math.Sqrt(3.0), 1.0 / Math.Sqrt(3.0) };
        double[] gaussWeights = { 1.0, 1.0 };

        for (int i = 0; i < n; i++)
        {
            double x_start = a + i * h;
            double x_end = a + (i + 1) * h;

            for (int j = 0; j < gaussNodes.Length; j++)
            {
                // Преобразование на текущий интервал
                double scale = (x_end - x_start) / 2.0;
                double shift = (x_start + x_end) / 2.0;
                double x = shift + scale * gaussNodes[j];
                double y = f(x);

                // Прямоугольник с весом Гаусса
                double rectHeight = y * gaussWeights[j];
                var rectX = new double[] { x_start, x_start, x_end, x_end };
                var rectY = new double[] { 0, rectHeight, rectHeight, 0 };

                var polygon = plt.Add.Polygon(rectX, rectY);
                polygon.FillColor = Colors.BlueViolet.WithAlpha(.5); // Полупрозрачный фиолетовый
                polygon.LineColor = Colors.BlueViolet;
                polygon.LineWidth = 1;

                // Точка Гаусса
                var point = plt.Add.Marker(x, y);
                point.Color = Colors.BlueViolet;
                point.MarkerSize = 8;
                point.MarkerShape = MarkerShape.FilledSquare;
            }
        }
    }

    static string ShortenMethodName(string fullName)
    {
        return fullName switch
        {
            "Метод средних прямоугольников" => "Прямоугольники",
            "Метод трапеций" => "Трапеции",
            "Метод Симпсона" => "Симпсон",
            "Правило 3/8" => "3/8",
            "Гаусс 2 узла" => "Гаусс-2",
            "Гаусс 3 узла" => "Гаусс-3",
            "Гаусс 4 узла" => "Гаусс-4",
            _ => fullName
        };
    }

    // Вспомогательная функция для вычисления f(x)
    static double f(double x)
    {
        return Math.Sin(100 * x) * Math.Exp(-x * x) * Math.Cos(2 * x);
    }

    static Color GetMethodColor(string methodName)
    {
        return methodName.ToLower() switch
        {
            string s when s.Contains("прямоугольник") => Colors.Red,
            string s when s.Contains("трапец") => Colors.Green,
            string s when s.Contains("симпсон") => Colors.Blue,
            string s when s.Contains("3/8") => Colors.Orange,
            string s when s.Contains("гаусс 2") => Colors.BlueViolet,
            string s when s.Contains("гаусс 3") => Colors.Brown,
            string s when s.Contains("гаусс 4") => Colors.Magenta,
            _ => Colors.Black
        };
    }

    static void PrintResults(List<MethodResult> results)
    {
        Console.WriteLine("\n" + new string('=', 60));
        Console.WriteLine("РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЯ ИНТЕГРАЛА");
        Console.WriteLine(new string('=', 60));
        Console.WriteLine("∫₀³ sin(100x) · exp(-x²) · cos(2x) dx");
        Console.WriteLine(new string('=', 60));

        // Сортируем по точности (близости к среднему)
        var average = results.Average(r => r.Value);
        var sortedResults = results.OrderBy(r => Math.Abs(r.Value - average)).ToList();

        foreach (var result in sortedResults)
        {
            var deviation = Math.Abs(result.Value - average);
            var accuracy = deviation / Math.Abs(average) * 100;
            Console.WriteLine($"{result.MethodName,-25} {result.Value:E10} (отклонение: {deviation:E2}, {accuracy:F2}%)");
        }

        Console.WriteLine(new string('=', 60));
        Console.WriteLine($"Среднее значение: {average:E10}");
        Console.WriteLine($"Максимальное отклонение: {results.Max(r => Math.Abs(r.Value - average)):E2}");
        Console.WriteLine(new string('=', 60));

        // Рекомендация по выбору метода
        var bestMethod = sortedResults.First();
        Console.WriteLine($"\nРекомендуемый метод: {bestMethod.MethodName}");
        Console.WriteLine($"Обоснование: наименьшее отклонение от среднего значения");
    }
}

public class FunctionPoint
{
    public double X { get; set; }
    public double Y { get; set; }
}

public class MethodResult
{
    public string MethodName { get; set; } = string.Empty;
    public double Value { get; set; }
}