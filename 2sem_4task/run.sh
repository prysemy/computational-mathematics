#!/bin/bash

# Цвета
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   2D Уравнение теплопроводности${NC}"
echo -e "${BLUE}   (метод Гаусса-Зейделя)${NC}"
echo -e "${BLUE}========================================${NC}"

# 1. Компиляция
echo -e "\n${YELLOW}[1/3] Компиляция C++...${NC}"
g++ -std=c++17 -O2 -o generate data_generator.cpp -lm

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Ошибка компиляции!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Успешно${NC}"

# 2. Запуск расчета
echo -e "\n${YELLOW}[2/3] Запуск расчета...${NC}"
echo -e "   Внимание: расчет для больших сеток (700, 1000) может занять время"

./generate

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Ошибка расчета!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Расчет завершен${NC}"

exit 0
