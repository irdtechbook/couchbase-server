= Couchbase Mobile内部機構

== リビジョン

リビジョンの概念と運用上の留意点はすでに説明してありますが、ここでは、Couchbase Mobile内部における、リビジョンの管理方法について解説します。

=== 概要

Couchbase Mobileは、ドキュメントが作成、更新、または削除されるたびにリビジョン(Revision@<fn>{revisions})を作成します。
ドキュメントの削除に対応するリビジョンは、トゥームストーン（墓石）リビジョンと呼ばれます。

//footnote[revisions][https://docs.couchbase.com/sync-gateway/current/revisions.html]

一意のドキュメントIDで識別されるドキュメントは、複数のリビジョンで構成され、各リビジョンには、一意のリビジョンIDが与えられます。

なお、ドキュメントの添付ファイルは、ドキュメント自体とは別に保存されますが、添付ファイルを変更してもリビジョンは生成されません。

=== リビジョンツリー

ドキュメントの変更履歴は、木構造からなるリビジョンツリーとして管理されます。
リビジョンツリーには、ドキュメントの存続期間を通じて発生した各リビジョンが順番に記録されます。

ツリーの先端であるリーフノードが、現在のリビジョン（ドキュメントの最新バージョン）です。

以下に、リビジョンツリーのイメージを示します。

//image[replication-protocol_1][リビジョンツリー概念図]{
//}

(図は、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0@<fn>{data-replication-couchbase-mobile}より引用)


=== プルーニング

リビジョンツリーが再現なく肥大しないよう、不要なリビジョンの削除が行われます。このプロセスは、プルーニング（剪定）といわれます。

リビジョンが追加されるたびに、プルーニングが自動的に実行されます。

たとえば、Sync Gatewayにおけるリビジョン保持数設定(@<tt>{revs_limit})が1000の場合、プルーニングは、最近の1000のリビジョンを保持し、他のリビジョンを削除します。

プルーニングは、Sync GatewayとCouchbase Liteとで、異なる方法で行われます。
それぞれのアルゴリズムについて、Couchbase Blog: Managing Database Sizes in Couchbase Mobile and Conflict Resolution@<fn>{database-sizes-and-conflict-resolution}にて詳説されています。

//footnote[database-sizes-and-conflict-resolution][https://blog.couchbase.com/database-sizes-and-conflict-resolution/]


====[column]リビジョンキャッシュ

ドキュメントへのアクセスが発生すると、リビジョンツリーがSync Gatewayにキャッシュされます。

エンタープライズエディションでは、@<tt>{database.cache.rev_cache}プロパティーを使用して、リビジョンキャッシュのサイズを設定できます。

この設定を調整することにより、Sync Gatewayのメモリー消費量を制御できます。
これは、メモリーが限られている環境を利用する場合、ドキュメント作成・更新が頻繁に発生する場合に重要になります。

====[/column]



== レプリケーションプロトコル

ここでは、Couchbase Mobileのレプリケーションプロトコルについて、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0@<fn>{data-replication-couchbase-mobile}の内容に基づいて解説します。

=== 概要
 
Couchbase Mobileのレプリケーションは、WebSocket上のメッセージングプロトコルとして実装されています。

WebSocketプロトコルは、単一のTCPソケット接続を介してリモートホスト間で全二重メッセージを渡すことができます。
レプリケーションプロトコルはHTTP/S接続として開始され、リモートホストのサポートが確認された後に、WebSocketに切り替わります。

=== チェックポイント

レプリケーションの最新の進行状況を記録することによって、レプリケーションが障害等による中断を経て再開する際に、レプリケーションの継続性を確実にするための仕組みを@<strong>{チェックポイント}と呼びます。

レプリケーションサイクルは、最後のチェックポイント以降のすべての変更を送信するプロセスであるといえます。
継続的レプリケーションでは、レプリケーションサイクル終了後、新たな変更の発生を待機の上、次のレプリケーションサイクルが実行されます。
ワンショットレプリケーションでは、レプリケーションサイクルが終了すると、処理は終了します。

チェックポイントは、JSONとしてエンコードされ、以下の要素から構成されています。 

//blankline

 * @<strong>{ローカルシーケンスID:} クライアントからサーバーにプッシュされた最後のシーケンスID
 * @<strong>{リモートシーケンスID:} クライアントがサーバーから受信した最後のシーケンスID


=== 接続確立

ここからは、レプリケーションにおけるフェーズ毎に、プロトコルのシーケンスを解説します。
なお、各シーケンスの記述においては、クライアント/サーバー間通信における基本的な構成に則って、レプリケーターを「クライアント」、Sync Gatewayを「サーバー」と記載します。

レプリケーションは、接続の確立からはじめられます。

//image[replication-protocol_2][レプリケーション接続確立プロセス]{
//}

(図は、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0より引用)

//blankline

 1. クライアントはHTTPプロトコルを用いて、サーバーにWebSocketハンドシェイク要求を送信し、WebSocketに切り替えようとしていることを伝えます。
 2. サーバーは、プロトコルの切り替えに同意したことを示して応答します。
 3. WebSocketによるハンドシェイクが行われると、HTTPの使用が停止され、すべての通信はWebSocketのメッセージになります。

//blankline

ひとつのソケットで、プッシュレプリケーションとプルレプリケーションの両方を同時にサポートできます。

=== チェックポイント確認


接続の確立に続いて、チェックポイントが確認されます。これはプッシュとプルのいずれの場合でも同様です。


//image[replication-protocol_3][チェックポイント確認プロセス]{
//}

(図は、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0より引用)

//blankline

  1. クライアントからサーバーに対して@<tt>{getCheckpoint}メッセージを送信します。このリクエストにはクライアントを識別するクライアントIDが含まれます。 
  2. サーバーから、@<tt>{getCheckpoint}要求への応答が返されます。この応答には、リクエストしたクライアントに対して、サーバー側で最後に記録されたチェックポイントが含まれます。
  3. クライアントは定期的に@<tt>{setCheckpoint}メッセージを送信し、チェックポイントを記録します。


=== プッシュレプリケーション

プッシュレプリケーションでは、ローカルデータベースにおける一連の変更が、クライアントからサーバーに送信されます。


//image[replication-protocol_4][プッシュレプリケーションプロセス]{
//}

(図は、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0@<fn>{data-replication-couchbase-mobile}より引用)

//blankline

  1. クライアントは、ローカルシーケンスID以降のローカルデータベースへの変更を検出すると、変更後のリビジョンに対応する@<tt>{change}オブジェクトの配列を含む@<tt>{proposeChanges}メッセージをサーバーに送信します。これには、トゥームストーンリビジョン（ドキュメント削除情報）も含まれます。
  2. @<tt>{proposeChanges}リクエストに対するサーバーの応答には、ステータスコードの配列を含むJSONオブジェクトが含まれます。 配列の各要素は、@<tt>{proposeChanges}リクエストで指定されたリビジョンIDに対応します。このステータスによって、そのリビジョンの状態(競合が発生しているかどうか等)が判別されます。
  3. クライアントは、要求されたリビジョンごとに@<tt>{rev}メッセージを送信します。@<tt>{rev}メッセージの本文にはJSON形式のドキュメントが含まれ、ヘッダーには関連するメタデータが含まれます。
  4. すべての@<tt>{rev}メッセージが送信された後、継続的レプリケーションでは、クライアントのレプリケーターはローカルデータベースが変更されるのを待ち、はじめのステップに戻ります。ワンショットレプリケーションでは、接続が切断され、レプリケーションが終了します。

=== プルレプリケーション


プルレプリケーションでは、クライアントからのプルリクエストに応答して、リモートデータベースにおける一連の変更が、サーバーからクライアントへ送信されます。


//image[replication-protocol_5][プルレプリケーションプロセス]{
//}

(図は、Couchbase Blog: Introducing the New Data Replication Protocol in Couchbase Mobile 2.0より引用)

//blankline

 1. クライアントは、@<tt>{subchanges}メッセージをサーバーに送信します。 @<tt>{subchanges}メッセージには、継続的レプリケーションかどうか等のレプリケーションを開始するにあたって必要な基本的な情報が含まれています。
 2. サーバーは、変更された現在の各リビジョンに対応する@<tt>{change}オブジェクトの配列@<tt>{changes}を含むメッセージをクライアントに送信します。
 3. クライアントは、受け取った変更のリストに対して、どの変更に関心があるか(要求するリビジョンのリスト)を応答します。その際、クライアントが受け入れることのできる履歴の最大サイズ(@<tt>{maxHistory})も伝達します。
 4. サーバーは、要求されたリビジョンごとに@<tt>{rev}メッセージを送信します。@<tt>{rev}メッセージの本文にはJSON形式のドキュメントが含まれ、ヘッダーには関連するメタデータが含まれます。
 5. 変更の送信が完了すると、サーバーは空の@<tt>{changes}メッセージを送信して、送信する変更がこれ以上ないことを示します。
 6. すべての変更が送信された後、継続的レプリケーションでは、サーバーが変更を待機している間、接続は開いたままになり、ステップ2に戻ります。ワンショットレプリケーションでは、接続が切断されレプリケーションが終了します。

//blankline

手順2〜4は、クライアントとサーバーの役割が入れ替わっているだけで、プッシュレプリケーションの手順1〜3と同じです。
このように、Couchbase MobileのWebSocketレプリケーションプロトコルは、クライアントとサーバーとで完全に異なるコードを必要とするHTTPベースのAPIとは異なり、対称的であるといえます。



//footnote[data-replication-couchbase-mobile][https://blog.couchbase.com/data-replication-couchbase-mobile/]