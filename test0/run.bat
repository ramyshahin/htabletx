for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0.exe %%x >> stats.txt
)
