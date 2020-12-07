
# AzureSphereExplorer

Azure Sphereのデバイスやプロダクト、デバイスグループなどをすばやく簡単に見ることができるツールとしてAzureSphereExplorerがあります。

```
Azure Sphere Explorerは、Azure Sphereを管理するGUIツールです。
Azure Sphereのデバイスやプロダクト、デバイスグループなどをすばやく簡単に見ることができます。
```

AzureSphereExplorerの詳細情報は[こちら](https://github.com/matsujirushi/AzureSphereExplorer)を参照してください。

アットマークテクノでは、AzureSphereExplorerをベースにして以下の機能を追加したAzureSphereExplorerの実行ファイルを提供します。

## 追記機能

### デバイスのクレーム (claim)

Azure Sphere テナントに接続できるようにするためには、Azure Sphere テナントにクレーム\[^注釈1]作業を行ってテナントにデバイスを紐付ける必要があります。AzureSphereExplorerでは、一度、クレームして Azure Sphere テナントに紐付けられると、紐付けられた Azure Sphere テナントを**変更することはできない**ため、注意して実施して下さい。

\[^注釈1]:クレームについての詳細についてはソフトウェアマニュアルを参照してください。
ソフトウェアマニュアルのダウンロードには、Armadilloサイトのユーザー登録とログインが必要になります。

https://armadillo.atmark-techno.com/resources/documents/cactusphere/manuals

![AzureSphereExplorer_DeviceClaimWindow_001](Images/AzureSphereExplorer_DeviceClaimWindow_001.png)

![AzureSphereExplorer_DeviceClaimWindow_002](Images/AzureSphereExplorer_DeviceClaimWindow_002.png)

![AzureSphereExplorer_DeviceClaimWindow_003](Images/AzureSphereExplorer_DeviceClaimWindow_003.png)

### SphereOS/アプリのOTA保留/時間指定実行

Cactusphereはアップデート保留機能\[^注釈2]を有しています。この機能を用いる事で、OSとFWのOTAを遅延させる事が出来ます。

\[^注釈2]:アップデート保留機能についての詳細についてはソフトウェアマニュアルを参照してください。
ソフトウェアマニュアルのダウンロードには、Armadilloサイトのユーザー登録とログインが必要になります。

https://armadillo.atmark-techno.com/resources/documents/cactusphere/manuals

![AzureSphereExplorer_DeviceClaimWindow_001](Images/AzureSphereExplorer_DeviceClaimWindow_001.png)

![AzureSphereExplorer_DeviceClaimWindow_002](Images/AzureSphereExplorer_DeviceClaimWindow_002.png)

![AzureSphereExplorer_DeviceClaimWindow_003](Images/AzureSphereExplorer_DeviceClaimWindow_003.png)


## 要件

### サポートしているプラットフォーム

* Microsoft Windows 10 (.NET Framework 4.8)
* Microsoft Windows 10 (.NET 5)

## インストール方法

1. [AzureSphereExplorer_AT.zip](https://github.com/KMOGAKI/Cactusphere-100/raw/explorer_rc-1/Tools/AzureSphereExplorer/AzureSphereExplorer_AT.zip)をダウンロードしてください。
1. 適当なフォルダに、`AzureSphereExplorer_AT.zip`を解凍してください。
1. 解凍した中にある、`AzureSphereExplorer.exe`を起動してください。
