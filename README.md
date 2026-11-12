# CANN Fuzzy Commitment + LFW ArcFace + AdaMTrans

这是此前合成数据版 Fuzzy Commitment 的独立扩展工程。它直接读取 LFW 的 512 维 ArcFace 特征，并将 WiFaKey 的 AdaMTrans 特征转换思路和 5G-NR BG2 LDPC 结构移植到 CANN / Ascend C。旧工程和旧 GitHub 仓库不会被覆盖。

## CANN 执行路径

本工程包含两个真正由 CANN CPU 仿真启动的 Ascend C 核：

1. `fuzzy_commitment_lfw_adamtrans`
   - 输入 512 维浮点 ArcFace 特征。
   - 使用四区间 LSSC 生成 1536 bit。
   - 应用固定、可复现的 AdaMTrans 随机掩码。
2. `fuzzy_commitment_lfw_ldpc_recover`
   - 使用 WiFaKey 同款 5G-NR Base Graph 2、`Z=10` 结构。
   - 每个 512 bit 块补 8 个零，形成 `N=520`、`K=100` 的 LDPC 块。
   - 通过归一化 Min-Sum 解码恢复 Commitment 码字。

Host 端负责 CSV 解析、helper data 构造、SHA-256 标签验证和独立参考实现比较。程序没有 CUDA、TensorFlow 或 PyTorch 运行依赖。

> WiFaKey 同时提出了带训练参数的 Neural-MS。当前版本移植了 AdaMTrans、BG2/Z=10 图结构和 Min-Sum 执行路径，但没有声称复现论文的完整训练过程或论文指标。

## 数据格式

默认特征文件：

```text
/home/dsh/下载/embeddings_arcface_r100_lfw.csv
```

要求：

- 12000 行，每两行组成一对 LFW 样本。
- 每行 512 个逗号分隔的浮点 ArcFace 特征。
- 特征行顺序与 `lfw_issame.csv` 一致。

默认标签文件：

```text
/home/dsh/cann-dev/biometric-data/lfw_issame.csv
```

标签共 6000 行，正负样本各 3000 对。本仓库不上传 150 MB 的人脸特征文件。

## 运行

当前虚拟机：

```bash
./run-tests.sh
```

自定义文件位置：

```bash
./run-tests.sh /path/to/embeddings.csv /path/to/lfw_issame.csv
```

若 CANN 不在默认位置：

```bash
CANN_ACTIVATE_SCRIPT=/path/to/cann/set_env.sh \
  ./run-tests.sh /path/to/embeddings.csv /path/to/lfw_issame.csv
```

## 当前真实数据测试

程序按 LFW 原始顺序选取：

- 前 5 对正样本：pair 0–4。
- 前 5 对负样本：pair 300–304。

测试不是挑选最容易的样本。当前结果：

```text
[RESULT] CANN/Host pipeline agreement: 10/10
[RESULT] Biometric classification: 10/10
[PASS] Fuzzy commitment LFW CANN pipeline completed.
```

分类正确率只作为观察值，不作为程序“能否跑通”的退出条件；CANN 与 Host 参考实现不一致才会使测试失败。

## 参数

| 项目 | 值 |
|---|---:|
| ArcFace 维度 | 512 |
| LSSC 位数 | 1536 |
| 掩码保留位 | 840 / 1536 |
| LDPC 基图 | 5G-NR BG2 Set 2 |
| 提升因子 Z | 10 |
| LDPC N / K / M | 520 / 100 / 420 |
| Tanner 图边数 | 1970 |
| 解码器 | Normalized Min-Sum |
| 最大迭代 | 50 |

## 局限

- 这是 CANN CPU 仿真与小样本协议验证，不代表真实昇腾 NPU 性能。
- 10 对样本不能用于报告生物识别准确率；完整评估应运行全部 6000 对并进行交叉验证。
- 当前掩码和阈值使用整份输入特征统计。正式评估应只在训练折拟合，再固定到测试折，避免数据泄漏。
- 固定掩码用于可复现测试，不是面向部署的密钥化、可撤销模板方案。
- 生产系统还需要分析 helper data 泄漏、熵、FMR/FNMR、可链接性和侧信道。

## 参考

- Xingbo Dong et al., [WiFaKey: Generating Cryptographic Keys from Face in the Wild](https://arxiv.org/abs/2407.14804)
- [WiFaKey 官方源代码](https://github.com/xingbod/WiFaKey)

第三方来源和版本记录见 `THIRD_PARTY_NOTICES.md`。

