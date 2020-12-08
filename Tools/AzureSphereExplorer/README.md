
# AzureSphereExplorer

Azure Sphereのデバイスやプロダクト、デバイスグループなどをすばやく簡単に見ることができるツールとして、AzureSphereExplorerがあります。AzureSphereExplorerの詳細情報は[こちら](https://github.com/matsujirushi/AzureSphereExplorer)を参照してください。

アットマークテクノでは、AzureSphereExplorerをベースにして、以下の機能を追加したAzureSphereExplorerの実行ファイルを提供します。

## 追記機能一覧

- デバイスのクレーム (claim) 機能
- SphereOS/アプリのOTA保留/時間指定実行

## 要件

### サポートしているプラットフォーム

* Microsoft Windows 10 (.NET Framework 4.8)
* Microsoft Windows 10 (.NET 5)

## インストール方法

1. [AzureSphereExplorer_AT.zip](https://github.com/KMOGAKI/Cactusphere-100/raw/explorer_rc-1/Tools/AzureSphereExplorer/AzureSphereExplorer_AT.zip)をダウンロードしてください。
1. 適当なフォルダに、`AzureSphereExplorer_AT.zip`を解凍してください。
1. 解凍した中にある、`AzureSphereExplorer.exe`を起動してください。


## 基本機能について

AzureSphereExplorerには3つのペイン(表示領域)があります。左ペインにProductとDevice Groupsの一覧が表示され、右ペインにはDevice一覧が表示されます。各項目の上で右クリックをすることで、項目の作成や削除、デプロイ設定、デバイスグループの変更などを行う事ができます。

<a href="../../Images/AzureSphereExplorer_MainWindow_002.png"><img src="../../Images/AzureSphereExplorer_MainWindow_002.png" width=70% style="display: block; margin: auto;"></a>

また、ProductかDevice Groupsの一覧上で右クリックし"Extract Devices from [Product/DeviceGroup]"を選択する事で、"ExtractWindow"を表示する事ができます。

<a href="../../Images/AzureSphereExplorer_ExtractWindow_001.png"><img src="../../Images/AzureSphereExplorer_ExtractWindow_001.png" width=70% style="display: block; margin: auto;"></a>

このウィンドウではProduct、Device Groups、Deviceを左から階層表示します。各プロダクトとデバイスグループに紐付いたデバイスの一覧を表示する事ができるため、全体の状態を把握する際に役立ちます。

また、デバイスグループ変更やクレームなどの機能では、デバイスIDが記載されたcsvファイルを読み込む事で一括設定する事も可能です。

## 追記機能の使い方

### デバイスのクレーム (claim) 機能

Azure Sphere テナントに接続できるようにするためには、Azure Sphere テナントにクレーム作業を行ってテナントにデバイスを紐付ける必要があります。AzureSphereExplorerでは、一度、クレームして Azure Sphere テナントに紐付けられると、紐付けられた Azure Sphere テナントを**変更することはできない**ため、注意して実施して下さい。

クレームについての詳細についてはソフトウェアマニュアルを参照してください。ソフトウェアマニュアルのダウンロードには、Armadilloサイトのユーザー登録とログインが必要になります。

https://armadillo.atmark-techno.com/resources/documents/cactusphere/manuals

### AzureSphereExplorerからデバイスのクレーム (claim) を行う方法

AzureSphereExplorerの"Device"タブから"Claim"を選択すると、"DeviceClaimWindow"が表示されます。

<a href="../../Images/AzureSphereExplorer_DeviceClaimWindow_001.png"><img src="../../Images/AzureSphereExplorer_DeviceClaimWindow_001.png" width=70% style="display: block; margin: auto;"></a>

"DeviceClaimWindow"の"Claim"ボタンを押下します。

"Attention"ウィンドウが表示され、クレームをしてよいか確認されます。**一度、クレームして Azure Sphere テナントに紐付けられると、紐付けられた Azure Sphere テナントを変更することはできないため、注意して実施して下さい。** 

問題ない場合は、「はい(Y)」ボタンを押下します。

<a href="../../Images/AzureSphereExplorer_DeviceClaimWindow_002.png"><img src="../../Images/AzureSphereExplorer_DeviceClaimWindow_002.png" width=70% style="display: block; margin: auto;"></a>

問題なくクレームが実行されると、"Claim is finished."と記載されたウィンドウが表示されます。その際、"DeviceClaimWindow"の"deviceids"の"success"の下に、クレームしたデバイスのデバイスIDが表示されている事を確認します。問題がない場合、"OK"ボタンを押下してウィンドウを閉じます。

<a href="../../Images/AzureSphereExplorer_DeviceClaimWindow_003.png"><img src="../../Images/AzureSphereExplorer_DeviceClaimWindow_003.png" width=70% style="display: block; margin: auto;"></a>

これでクレームは完了です。

### SphereOS/アプリのOTA保留/時間指定実行

Cactusphereはアップデート保留機能を有しています。この機能を用いる事で、OSとFWのOTAを遅延させる事ができます。

アップデート保留機能についての詳細についてはソフトウェアマニュアルを参照してください。
ソフトウェアマニュアルのダウンロードには、Armadilloサイトのユーザー登録とログインが必要になります。
https://armadillo.atmark-techno.com/resources/documents/cactusphere/manuals

### AzureSphereExplorerからOTA保留機能を利用する方法

AzureSphereExplorerの"Devices"ペイン内のデバイスID一覧から任意のデバイスを選び、 **右クリック** します。一覧から"Update Properties"を選択します。

<a href="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_001.png"><img src="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_001.png" width=70% style="display: block; margin: auto;"></a>

"UpdatePropertiesWindow"が表示されます。アップデート保留機能を利用する際、IoT CentralアプリケーションのURLとAPI トークンの値が必要になります。

<a href="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_003.png"><img src="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_003.png" width=70% style="display: block; margin: auto;"></a>

* ①："CentralAppName:"に、IoT CentralアプリケーションのURLを入力します。IoT CentralアプリケーションのURLはIoT Centralの「管理」→「お客様のアプリケーション」→「アプリケーションURL」で確認する事ができます。

<a href="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_009.png"><img src="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_009.png" width=70% style="display: block; margin: auto;"></a>

* ②："APIToken:"に、API トークンを入力します。API トークンはIoT Centralの「管理」→「API トークン」→「トークンの生成」で生成した際に表示される値です。（"sr="より後の数値を入れて下さい）API トークンは一度しか表示されないため、既に表示させてAPIトークンの値が不明な場合は、もう一度任意の名前でAPIトークンを作成し、その値を設定して下さい。

<a href="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_005.png"><img src="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_005.png" width=70% style="display: block; margin: auto;"></a>


"FW Update Time"にはファームウェア、"OS Update Time"にはOSをアップデートする時間を設定する事ができます。

<a href="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_004.png"><img src="../../Images/AzureSphereExplorer_UpdatePropertiesWindow_004.png" width=70% style="display: block; margin: auto;"></a>

この際、startの時間とendの時間を同じにすることはできませんので、注意が必要です。

"Update"ボタンを押下して、アップデート保留機能を設定します。
"OK"と表記されたウィンドウが表示された場合、アップデート保留機能の設定に成功しています。

これで、アップデート保留機能の設定は完了です。