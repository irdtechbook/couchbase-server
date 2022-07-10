
= ピアツーピア同期

エンタープライズエディションで提供される機能から、独自のユースケースに対応する機能を紹介します。

//blankline

Couchbase Liteのピアツーピア同期(Data Sync Peer-to-Peer@<fn>{p2psync-websocket})は、エッジデバイス間でデータの双方向同期を行うアプリケーションの開発を容易にします。

== 背景

ピアツーピア(Peer-to-peer/P2P@<fn>{wiki-Peer-to-peer})という表現は、ピア(対等者)同士が直接的に相対している、そのようなアーキテクチャーを示しています。
これは、クライアントサーバーモデル@<fn>{wiki-client-server}と対置される形で示されることが一般的です。

P2P通信の基本的な概念としては、あくまでコンピュータ端末の役割について規定しているものであり、例えばSkypeのようなサービスでP2Pアーキテクチャーが採用される場合、あるいはWinnyのようなファイル共有ソフトでP2Pネットワークが用いられる場合には、インターネット環境の利用を前提としていますが、モバイル/IoTに関連してP2Pが語られる際には、端末に備わっているWi-FiやBluetooth機能を利用した、オフライン環境での端末同士の直接的な通信というコンテクストで用いられるケースがみられます。
そのようなアプリケーションの一つにFireChat@<fn>{wiki-FireChat}があります。

FireChatについては、香港のデモ参加者に利用された、あるいはイラク政府によるインターネット規制環境下で用いられたという報道で名前が知られたところがありますが、モバイルアプリにおけるP2P通信活用については、自然災害発生時のように通信インフラが不安定化している状況における連絡手段としても注目されています。

Couchbase Liteのピアツーピア同期も、そのようなオフライン環境での端末間での直接的な通信、この場合はローカルデータベース間のデータの同期、を実現するものです。

なお、モバイルアプリにおける上記のようなP2P通信機能と同時に、メッシュネットワーキング(@<fn>{wiki-Mesh_networking})について触れられることが見られますが、Couchbase Liteのピアツーピア同期は、あくまでアクティブピアとパッシブピア間のデータ同期機能を提供するものであり、メッシュネットワークを実現する機能の設計・実装については、アプリケーションサイドの担当範囲となります。


//footnote[wiki-Peer-to-peer][https://en.wikipedia.org/wiki/Peer-to-peer]
//footnote[wiki-client-server][https://en.wikipedia.org/wiki/Client%E2%80%93server_model]
//footnote[wiki-FireChat][https://en.wikipedia.org/wiki/FireChat]
//footnote[wiki-Mesh_networking][https://en.wikipedia.org/wiki/Mesh_networking]

== 概要


Couchbase Liteのピアツーピア同期では、開発者が自身でピアツーピア同期を実装する場合と比べて、以下のような機能と利点を提供します。

//blankline

 * 通信暗号化や認証を備えたセキュアな通信
 * データ同期における競合発生時の自動競合解決
 * ネットワーク障害、一時停止からのリカバリー
 * デルタ同期による、ネットワーク帯域幅使用の最適化とデータ転送コスト削減

//blankline

Couchbase Liteのピアツーピア同期は、たとえば、AndroidとiOSのようなクロスプラットフォーム間の同期をサポートしています。


//footnote[p2psync-websocket][https://docs.couchbase.com/couchbase-lite/current/android/p2psync-websocket.html]


== アーキテクチャー


以下に、Couchbase Liteピアツーピア同期の基本的なアーキテクチャーを示します。


//image[peer-to-peer][Couchbase Liteピアツーピア同期アーキテクチャー]{
//}

(図は、Couchbaseドキュメント Data Sync Peer-to-Peerより引用)

//blankline

このように、Couchbase Liteのピアツーピア同期は、リスナー(パッシブピア、またはサーバー)とレプリケーター(アクティブピア、またはクライアント)との間で実現されます。


//footnote[nsd][https://developer.android.com/training/connect-devices-wirelessly/nsd]

=== 開発参考情報

iOSとXamarin用の具体的な実装について、以下のチュートリアルを参照することができます。

//blankline

 * Getting Started with Peer-to-Peer Sync on iOS@<fn>{cbl-p2p-sync-websocket-swift}
 * Getting Started with Peer-to-Peer Sync on Xamarin (UWP, iOS, and Android)@<fn>{cbl-p2p-sync-websockets-dotnet}

//footnote[cbl-p2p-sync-websocket-swift][https://docs.couchbase.com/tutorials/cbl-p2p-sync-websockets/swift/cbl-p2p-sync-websockets.html]


//footnote[cbl-p2p-sync-websockets-dotnet][https://docs.couchbase.com/tutorials/cbl-p2p-sync-websockets/dotnet/cbl-p2p-sync-websockets.html]



