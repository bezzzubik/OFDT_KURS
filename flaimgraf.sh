#!bin/bash

perf record -g -o perf.data ./a.out

# 2. Обработка данных
perf script > perf.unfold

# 3. Преобразование данных в формат Flame Graph
cd FlameGraph
./stackcollapse-perf.pl ../perf.unfold > perf.folded

# 4. Генерация Flame Graph
./flamegraph.pl perf.folded > flamegraph.svg