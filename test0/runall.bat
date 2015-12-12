for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_0.exe %%x >> stats_diag_retry0.txt
)

for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_1.exe %%x >> stats_diag_retry1.txt
)

for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_2.exe %%x >> stats_diag_retry2.txt
)

for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_3.exe %%x >> stats_diag_retry3.txt
)

for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_4.exe %%x >> stats_diag_retry4.txt
)

for /l %%x in (1, 1, 32) do (
  for /l %%y in (1, 1, 10) do test0_5.exe %%x >> stats_diag_retry5.txt
)
