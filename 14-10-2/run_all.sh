#!/bin/bash

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Запуск полного цикла расчета${NC}"
echo -e "${BLUE}========================================${NC}"

# Проверяем наличие необходимых файлов
if [ ! -f "data_generator.cpp" ]; then
    echo -e "${RED}Ошибка: data_generator.cpp не найден!${NC}"
    exit 1
fi

if [ ! -f "plotter.py" ]; then
    echo -e "${RED}Ошибка: plotter.py не найден!${NC}"
    exit 1
fi

# Создаем папки, если их нет
echo -e "${YELLOW}Создание директорий...${NC}"
mkdir -p data
mkdir -p plots

# Компиляция C++ кода
echo -e "${YELLOW}Компиляция data_generator.cpp...${NC}"
g++ -std=c++17 -O3 data_generator.cpp -o data_generator

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo -e "${GREEN}Компиляция успешна${NC}"
echo -e "${BLUE}----------------------------------------${NC}"

# Запуск C++ программы
echo -e "${YELLOW}Запуск вычислений...${NC}"
./data_generator

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка при выполнении вычислений!${NC}"
    exit 1
fi

echo -e "${GREEN}Вычисления завершены${NC}"
echo -e "${BLUE}----------------------------------------${NC}"

# Проверяем, создались ли CSV файлы
csv_count=$(ls -1 data/*.csv 2>/dev/null | wc -l)
if [ "$csv_count" -eq 0 ]; then
    echo -e "${RED}Ошибка: CSV файлы не созданы!${NC}"
    exit 1
fi

echo -e "${YELLOW}Найдено $csv_count CSV файлов${NC}"

# Запуск Python скрипта
echo -e "${YELLOW}Запуск визуализации...${NC}"

python3 plotter.py

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка при выполнении визуализации!${NC}"
    exit 1
fi

echo -e "${GREEN}Визуализация завершена${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Все готово!${NC}"
echo -e "${YELLOW}Результаты:${NC}"
echo -e "  - CSV файлы: ${BLUE}data/${NC}"
echo -e "  - Графики:   ${BLUE}plots/${NC}"
echo -e "${BLUE}========================================${NC}"
