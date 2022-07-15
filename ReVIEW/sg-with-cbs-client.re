
= Couchbase Serverクライアントとの共存

== 概要

Couchbase Serverを、単にCouchbase Mobileによるモバイル/エッジコンピューティングデータプラットフォームのバックエンドデータベースとして利用するだけでなく、同時に、Webアプリケーションのような他のシステムのデータベースとしても利用し、共通のデータに基づくサービスをユーザーに提供することは珍しくありません。

Sync Gatewayは、Couchbase Serverを単体で利用している場合には存在しない、Couchbase Liteとの同期のためのデータを、Couchbase Serverのバケットに保持します。
Couchbase Liteと同期されるドキュメントには、ユーザーが定義したドキュメントプロパティー以外の、同期のためのプロパティー/拡張属性が存在します。
そのため、Couchbase Serverクライアント(Couchbase Server SDKを利用するアプリケーション)が、Couchbase Serverに保存したドキュメントを、Sync Gatewayを介してCouchbase Liteと同期するためには、これらのプロパティー/拡張属性のための追加的な措置が必要になります。

また、必ずしもCouchbase Serverクライアントアプリケーションが扱うデータの全てをCouchbase Liteと同期する必要があるとは限らないため、同期の範囲を定める必要も生じます。 

ここでは、Couchbase Mobileクライアントに対してだけではなく、Couchbase Serverクライアントに対してもホストしているCouchbase Serverと同期するケース(Sync with Couchbase Server@<fn>{sync-with-couchbase-server})について解説します。

//footnote[sync-with-couchbase-server][https://docs.couchbase.com/sync-gateway/current/sync-with-couchbase-server.html]



== 共有バケットアクセス

Couchbase Liteアプリケーションと、Couchbase Serverクライアントが同じバケットに対して読み取りと書き込みを行えるようにする仕組みを@<strong>{共有バケットアクセス}と呼びます。

Sync Gateway構成プロパティー@<tt>{enable_shared_bucket_access}の設定により、共有バケットアクセスを有効化します。

以下に、設定例を示します。


//cmd{
$ curl -vX  PUT "http://localhost:4985/mybucket/_config" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -H 'Content-Type: application/json'  -H "accept: application/json" \
  -d '{ "enable_shared_bucket_access" : true, "import_docs": true }' 

//}


== インポート処理

Sync Gatewayが、Couchbase Serverクライアントにより行われたデータの登録・変更を認識し、そのデータをCouchbase Liteと同期するために必要とされるメタデータを設定する処理は、@<strong>{インポート処理}(Import Processing)と呼ばれます。

=== 構成

Sync Gatewayを複数ノードからなるクラスターとして構成する場合、ノードをインポート処理に参加させるかどうかを選択することができます。

ノードがインポート処理に参加することを指定するために@<tt>{import_docs}プロパティーが使用されます。
なお、@<tt>{enabled_shared_bucket_access}が@<tt>{true}に設定されている場合にのみ@<tt>{import_docs}が有効になります。

インポート処理においても、ドキュメントに対してSync関数が適用されますが、通常のSync関数の適用とは次の違いがあります。

//blankline

 * インポートは、管理REST APIを介して行われる変更と同様に、管理者ユーザーのコンテキストで処理されます。そのため、Sync関数内で@<tt>{requireAccess}、@<tt>{requireUser}および@<tt>{requireRole}が利用されている場合、それらは、通常のユーザーコンテキストのようには評価されず、常に成功します。
 * Sync関数の引数@<tt>{oldDoc}は@<tt>{null}になります。





=== インポートフィルター

大量のデータを含むクラスターの場合、最初のインポートプロセスが完了するまでに相当の時間がかかることがあります。
インポートフィルター(Import Filter@<fn>{import-filter})を使用することにより、このプロセスを効率化することができます。
インポートフィルターを利用すると、特定のドキュメントのみをインポート対象(Couchbase Liteとの同期対象)とすることができます。

//footnote[import-filter][https://docs.couchbase.com/sync-gateway/current/import-filter.html]


インポートフィルターは、次のような目的で使うことが考えられます。

//blankline

 * ドキュメントの種類に応じて、モバイルでの利用の必要有無を反映させる。
 * ドキュメントの作成年月日に応じて、モバイルでの利用の必要有無を反映させる。
 * アプリケーションが設定するフラグによって、インポート処理実行の必要性を反映させる。

//blankline

インポートフィルターは、以下のようなJavaScript関数として定義します。
この例では、「type」プロパティーが、「mobile」に等しいとドキュメントのみをインポートします。

//emlist[][JAVASCRIPT]{
function(doc) {
    if (doc.type != "mobile") {
        return false;
    }
    return true;
}
//}

インポートフィルターの設定には、以下のように管理REST APIを利用します。
@<tt>{import_filter}プロパティーを使用してフィルター関数を指定します。

//cmd{
$ curl -X PUT "http://localhost:4985/mybucket/_config/import_filter" \
  -H "accept: application/json" \
  -H "Content-Type: application/javascript" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d 'function(doc) { if (doc.type != "mobile") { return false; } return true; }'
//}

====[column]@<tt>{import_docs}デフォルト設定

コミュニティーエディションでは、@<tt>{import_docs}プロパティーのデフォルトは@<tt>{false}です。
ノードをインポート処理に参加させるには、明示的に@<tt>{true}に設定する必要があります。

エンタープライズエディションでは、@<tt>{import_docs}のデフォルトは@<tt>{true}であり、クラスタ内のすべてのノードがインポート処理に参加することを意味します。
ノードをインポート処理から除外するには、@<tt>{import_docs}プロパティーを@<tt>{false}に設定します。

詳細は、ドキュメント@<fn>{sync-with-couchbase-server-configuration}を参照してください。

====[/column]

//footnote[sync-with-couchbase-server-configuration][https://docs.couchbase.com/sync-gateway/current/sync-with-couchbase-server.html#configuration]


=== ワークロード分離

ノードをインポート処理専用に構成し、クライアントからのリクエストを処理するノードから分離することができます。

@<tt>{import_docs}が@<tt>{false}に設定されている場合、そのノードはインポートプロセスに参加しません。
 
 この構成は、ワークロードの分離に寄与します。高い書き込みスループットが想定される場合、ワークロードの分離が望ましい場合があります。




次の図は、クライアント接続を処理するふたつのSync Gatewayノードとインポート処理専用のふたつのノードからなるクラスター構成を示しています。


//image[import_docs-diagram][Sync Gatewayワークロード分離アーキテクチャー例]{
//}

(図は、CouchbaseドキュメントSync with Couchbase Serverより引用)

====[column]エディションによるインポート処理の高可用性対応の違い

Sync Gatewayコミュニティーエディションには、インポート処理をノード間で分散する機能が存在しません。高可用性のためにはスタンバイノードを用いて、外部的に制御する必要があります。

Sync Gatewayエンタープライズエディションでは、インポート処理作業は、インポート処理を有効に設定されたすべてのSync Gatewayノードにわたって分散されます。これによって、インポート処理の高可用性が実現されます。

====[/column]




== 留意事項

=== トゥームストーンリビジョン

ドキュメントの削除に対応するリビジョンは、トゥームストーン(墓石)リビジョンと呼ばれます。
バケット共有を有効にする場合、トゥームストーンリビジョンの存在に注意する必要があります。

トゥームストーンリビジョンは、Couchbase Serverのメタデータパージ間隔に基づいてパージされます。
トゥームストーンリビジョンがCouchbase Liteに同期される前にパージされると、Couchbase Liteからドキュメントが削除されるタイミングを失うことになります。

トゥームストーンリビジョンが確実に同期されるようにするには、レプリケーション頻度に基づいてCouchbase Serverのメタデータパージ間隔を設定する必要があります。

詳細は、ドキュメント@<fn>{managing-tombstones}を参照ください。

//footnote[managing-tombstones][https://docs.couchbase.com/sync-gateway/current/managing-tombstones.html]

=== 添付ファイル操作

Couchbase Liteでは、バイナリデータをドキュメントの添付ファイルとして扱うことができます。
添付ファイルはCouchbase Lite独自のコンセプトであり、Couchbase Serverには、ドキュメントの添付ファイルという概念は存在しません。

Couchbase Serverでは、バイナリデータをドキュメントとして保存することができます。
Couchbase Mobileは、この機能を利用して、Couchbase Liteの添付ファイル付きドキュメントとCouchbase Serverとの同期を行っています。

Sync Gatewayでは、添付ファイルを操作するパブリックREST APIが提供されています。
この機能を利用して、Couchbase Liteの添付ファイル付きドキュメントによって実現されているモバイルアプリ上の機能と同等の操作を、Webアプリケーション等の他のプラットフォームに対して提供することが可能になっています。
Couchbase Blog: Handle Binary Data Attachments & Blobs with Couchbase Mobile@<fn>{store-sync-binary-data-attachments-blobs-couchbase-mobile}で実際的な利用例を参照することができます。
 
//footnote[store-sync-binary-data-attachments-blobs-couchbase-mobile][https://blog.couchbase.com/store-sync-binary-data-attachments-blobs-couchbase-mobile]

