## 改动
1. 使用map优化closelist，closelist为局部变量以避免污染
2. 使用优先队列优化openlist
3. 调整visited判定顺序，减少重复运算