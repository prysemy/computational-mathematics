#!/bin/bash

# Название скрипта: run_simulation.sh
# Назначение: Компилирует и запускает data_generator.cpp, затем запускает plotter.py

# Цвета для вывода
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   Задача о распаде разрыва (газовая динамика)${NC}"
echo -e "${BLUE}========================================${NC}"

# 1. Компиляция data_generator.cpp
echo -e "\n${YELLOW}[1/4] Компиляция data_generator.cpp...${NC}"

if [ -f "data_generator.cpp" ]; then
    g++ -std=c++17 -O2 data_generator.cpp -o data_generator
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Компиляция успешна!${NC}"
    else
        echo -e "${RED}✗ Ошибка компиляции!${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Файл data_generator.cpp не найден!${NC}"
    exit 1
fi

# 2. Запуск программы для генерации данных
echo -e "\n${YELLOW}[2/4] Запуск расчета...${NC}"
echo -e "${BLUE}   Параметры:${NC}"
echo -e "   - Область: [-10, 10] м"
echo -e "   - Конечное время: 0.02 с"
echo -e "   - Число узлов: 101"
echo -e "   - CFL: 0.001"

./data_generator

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Расчет завершен успешно!${NC}"
    
    # Проверяем, создалась ли папка с данными
    if [ -d "gas_data" ]; then
        NUM_FILES=$(ls -1 gas_data/*.csv 2>/dev/null | wc -l)
        echo -e "   Создано файлов: ${NUM_FILES}"
    else
        echo -e "${RED}✗ Папка gas_data не создана!${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Ошибка при выполнении расчета!${NC}"
    exit 1
fi

# 3. Проверка наличия plotter.py
echo -e "\n${YELLOW}[3/4] Проверка plotter.py...${NC}"

if [ -f "plotter.py" ]; then
    echo -e "${GREEN}✓ plotter.py найден${NC}"
else
    echo -e "${RED}✗ Файл plotter.py не найден!${NC}"
    exit 1
fi

# 4. Запуск Python скрипта для построения графиков
echo -e "\n${YELLOW}[4/4] Построение графиков...${NC}"

# Проверяем наличие Python
if command -v python3 &>/dev/null; then
    python3 plotter.py
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Графики построены успешно!${NC}"
        
        # Проверяем, создалась ли папка с графиками
        if [ -d "gas_plots" ]; then
            echo -e "   Графики сохранены в папке gas_plots/"
            ls -la gas_plots/
        else
            echo -e "${YELLOW}   Папка gas_plots не найдена, возможно графики не были созданы${NC}"
        fi
    else
        echo -e "${RED}✗ Ошибка при построении графиков!${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Python3 не найден! Установите Python3.${NC}"
    exit 1
fi

# 5. Вывод итоговой информации
echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN}   ВСЕ ЗАДАЧИ ВЫПОЛНЕНЫ!${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "\nРезультаты сохранены в:"
echo -e "  📁 gas_data/     - файлы с данными (CSV)"
echo -e "  📁 gas_plots/    - графики (PNG, PDF, GIF)"
echo -e "\nДля просмотра анимации:"
echo -e "  ${BLUE}open gas_plots/gas_evolution.gif${NC} (macOS)"
echo -e "  ${BLUE}xdg-open gas_plots/gas_evolution.gif${NC} (Linux)"
echo -e "  ${BLUE}start gas_plots/gas_evolution.gif${NC} (Windows WSL)"
echo ""

# 6. Опционально: показать график плотности в терминале (если есть imgcat)
if command -v imgcat &>/dev/null; then
    echo -e "${YELLOW}Предпросмотр графика плотности:${NC}"
    if [ -f "gas_plots/final_state.png" ]; then
        imgcat gas_plots/final_state.png
    fi
fi

exit 0
