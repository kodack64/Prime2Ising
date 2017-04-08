# Prime2Ising

素因数分解から帰着した落とし戸付きのイジング模型のハミルトニアンを計算します。

FACTORING -> Circuit SAT -> MAXSAT -> QUBO -> MAXCUT -> Ising

の順に経由して隣接相互作用のみのハミルトニアンをO(n^2)で作ります。
FACTORINGからC-SATへの帰着で工夫をするとO(n log n)にできます。

4=2*2を変換したMAXCUT

![4=2*2](ising.png)
