#!/bin/bash

# Цвета для вывода
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   Плоскопараллельная однофазная фильтрация${NC}"
echo -e "${BLUE}   (вязкоупругая жидкость, неявная схема)${NC}"
echo -e "${BLUE}========================================${NC}"

# 1. Компиляция solver.cpp
echo -e "\n${YELLOW}[1/4] Компиляция solver.cpp...${NC}"

if [ -f "data_generator.cpp" ]; then
    g++ -std=c++17 -O2 -Wall data_generator.cpp -o generate -lm
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Компиляция успешна!${NC}"
    else
        echo -e "${RED}✗ Ошибка компиляции!${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Файл solver.cpp не найден!${NC}"
    exit 1
fi

# 2. Запуск расчёта
echo -e "\n${YELLOW}[2/4] Запуск численного моделирования...${NC}"
echo -e "${CYAN}   Параметры задачи:${NC}"
echo -e "   - Длина пласта:     500 м"
echo -e "   - Проницаемость:    10 мД (1e-14 м²)"
echo -e "   - Вязкость:         1 сПз (1e-3 Па·с)"
echo -e "   - Пористость:       0.2"
echo -e "   - Сжимаемость:      1e-4 атм⁻¹"
echo -e "   - Узлов по пространству: 101"
echo -e "   - Шаг по времени:   1 час"
echo -e "   - Время моделирования: 10 дней"
echo -e "   - Граничные условия: p(0)=150 атм, p(500)=50 атм"

mkdir -p output

./generate

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Расчёт завершён успешно!${NC}"
    
    if [ -d "output" ]; then
        NUM_FILES=$(ls -1 output/profile_*.dat 2>/dev/null | wc -l)
        echo -e "   Сохранено профилей давления: ${NUM_FILES}"
    else
        echo -e "${RED}✗ Папка output не создана!${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Ошибка при выполнении расчёта!${NC}"
    exit 1
fi

exit 0
