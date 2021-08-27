
= Node.jsアプリケーション開発

== なぜ、JavaScriptとNoSQLの組み合わせなのか？


NoSQLは、非RDB（RDB以降の技術）であることのみを共通点として、様々な異なる特色を持った技術に対する総称となっていますが、ここで対象とするカテゴリーは、その中でもドキュメント指向データベース（ドキュメントストア）と呼ばれるものです。



ドキュメントストアは、JSONデータを格納することを特徴としており、自ずと、JavaScriptと親和性を持っています(JSON = Java Script Object Notation)。



その一方、RDBから移行するだけの利点があるのか、というのが実際的な関心なのではないかと思います。こちらについては後ほど触れていきます。





== NoSQL/Couchbaseを選択する理由


Couchbaseなら、JSONデータとクエリ言語の両方の利点を活用することができます。


=== JSONデータの利点


これは、RDBの欠点と見ることもできます。つまり...

 * アプリケーションが必要とするデータ構造（ドメインオブジェクト）と、RDBが要請する形式（第一正規形テーブル構造）との間には、断絶がある。
 * アプリケーションの設計、実装、改善、機能追加など、全ての工程において、データベースとの兼ね合いを図る必要がある（密結合）



これに対して、データ層が、JSONを許容した場合...

 * データ層は、第一正規形を要請しないため、アプリケーションが必要とするデータ構造（ドメインオブジェクト）そのものを格納することができる。
 * （JSONには、データ構造に関する情報がデータ自体に含まれているため）アプリケーション設計・開発工程において、特にデータ設計の変化に（データベース側の作業を伴うことなく）柔軟に対応できる


=== クエリ言語の利点


これは、RDBの持つ大きな利点であり、標準化されたクエリ言語(SQL)が様々な異なるデータベースで利用できることは、技術者層の拡大に繋がり、SQLの習得は、（特にオープンシステムのWEBアプリケーション全盛時代には）システム開発者にとって、必須知識といえるものとなっていました。



Couchbase Serverを選択することで、開発者は、SQLの知識を活用することができます。


== キーバリュー操作

他の箇所で、N1QLによるクエリについてRDBとの相違点を中心に解説しました。Couchbase Serverでは、クエリが唯一のインターフェイスではなく、キーバリューストア同様にキー指定によるドキュメントの操作を行うことが可能です。

=== CRUD操作

キーバリューの基本的な操作について、プログラミング例を参考として示します。詳細についてはドキュメント@<fn>{kv-operations}を参照ください。

==== CREATE

//emlist[][js]{
const result = await collection.insert(key, document);
//}

==== READ

//emlist[][js]{
const result = await collection.get(key);
document = result.value;
//}

==== UPDATE

//emlist[][js]{
let result = await collection.replace(key, document);
//}

==== UPSERT

//emlist[][js]{
let result = await collection.upsert(key, document);
//}

==== DELETE

//emlist[][js]{
const result = await collection.remove(key);
//}


//footnote[kv-operations][https://docs.couchbase.com/nodejs-sdk/current/howtos/kv-operations.html]

=== サブドキュメント操作

純粋なキーバリューストアと異なり、JSONデータベースを利用している場合、アプリケーションはJSONドキュメントの特定の箇所のみを取得すれば十分な場合があります。
あるいは、アプリケーションでデータの更新を行う場合に、ドキュメント全体を取得し、その一部を編集した後に、ドキュメントをデータベースに対して更新するのではなく、データベースに対してドキュメントの指定箇所の更新を指示できることが重要です。このような操作は、N1QLを利用しても行うことができますが、それは唯一の方法ではありません。
Couchbase Serverでは、DataサービスAPIを用いて、JSONデータの一部の取得・更新を行うことができます。これをサブドキュメント操作と呼びます。

サブドキュメント操作の基本について、プログラミング例を参考として示します。詳細についてはドキュメント@<fn>{subdocument-operations}を参照ください。

//footnote[subdocument-operations][https://docs.couchbase.com/nodejs-sdk/current/howtos/subdocument-operations.html]

==== READ

//emlist[][js]{
 var result = await collection.lookupIn("customer123", [
    couchbase.LookupInSpec.get("addresses.delivery.country"),
  ]);
  var country = result.content[0].value; //'United Kingdom'
//}

==== EXISTS

サブドキュメントを実際に取得しなくても、指定したパスのサブドキュメントを存在するかどうかを確認することが可能です。

//emlist[][js]{
  var result = await collection.lookupIn("customer123", [
    couchbase.LookupInSpec.exists("purchases.pending[-1]"),
  ]);
  console.log("Path exists? ", result.content[0].value);

//} 

==== 複数操作の同時実行

//emlist[][js]{

  var result = await collection.lookupIn("customer123", [
    couchbase.LookupInSpec.get("addresses.delivery.country"),
    couchbase.LookupInSpec.exists("purchases.pending[-1]"),
  ]);

  console.log(result.content[0].value); 
  console.log("Path exists?", result.content[1].value);

//}

==== CREATE

//emlist[][js]{
  await collection.mutateIn("customer123", [
    couchbase.MutateInSpec.insert("purchases.complete", [42, true, "None"]),
  ]);

//}



==== UPDATE

//emlist[][js]{
  await collection.mutateIn("customer123", [
    couchbase.MutateInSpec.replace("email", "dougr96@hotmail.com"),
  ]);
//}

==== UPSERT

//emlist[][js]{
  await collection.mutateIn("customer123", [
    couchbase.MutateInSpec.upsert("fax", "311-555-0151"),
  ]);
//}

==== DELETE

//emlist[][js]{
  await collection.mutateIn("customer123", [
    couchbase.MutateInSpec.remove("addresses.billing"),
  ]);
//}

== データ一貫性(Consistency)

=== 分散アーキテクチャーにおけるインデックス


Couchbase ServerとRDBMSとの違いとして、メモリーファーストアーキテクチャーがあります。
メモリーファーストアーキテクチャーは、永続化装置および複製（レプリケ）の作成が、アプリケーション（への応答）性能のボトルネックになることを回避するための仕組みです。そのための仕組みとして、Couchbase Serverは、ノード内部でディスクへの書き込みのためのキューを持っています。また、他ノードへの複製（レプリカ）のためのキューメカニズムとして、Data Change Protocol(DCP)と呼ばれる仕組みを持っています。
このDCPは、レプリカのためのみではなく、クラスター内部でデータの変更を他のノードやサービスに反映するために広く利用されます。（Dataサービスにおける）データ変更のインデックスへの反映もその一つです。


=== インデックス更新から見るRDBとの違い


Couchbase ServerとRDBとの違いをこの観点から見ることができます。データを更新する際には、関係するインデックスの更新が行われるのは、Couchbase ServerとRDBとで共通です。ただし、RDBでは、ユーザーが、いつどのように、データへアクセスしたとしても、データの一貫性が保たれていることを保証するため、クライアントからのデータ更新処理リクエストに対して、成功のステータスが返されるのは、インデックスの更新が完了した後になります。これは、データ一貫性の保証という要件面に加え、RDBがモノリシックなアークテクチャーからなっているという、技術的な面から見ても自然な挙動と言えるかもしれません。一方で、Couchbase Serverは、分散アーキテクチャーという性格を持ち、データの更新処理を司るDataサービスと、インデックスを管理するIndexサービスとは、コンポーネントとして独立しており、異なるサーバーに配置されるのが一般的です。Couchbase Servevrでは、インデックスの更新は、永続化装置への反映や、複製の作成と同様、DCPプロトコルを介した、キュー（非同期）のメカニズムで実現されています。


=== インデックス更新が非同期であることの影響範囲


RDBにとって、SQLがクライアントにとって共通のインターフェースであるのと異なり、Couchbase Serverでは、N1QLは、クライアントにとって、選択することのできる複数のインターフェースのうちの1つです。さらに言えば、Dataサービスへのキーバリューアクセスが、一次的なインターフェイスであるのに対して、あくまで派生的なインターフェースであると言えます。キーによるアクセスでは、常にデータの同期が保証されます（たとえ、ある新しい更新が、永続化されておらず、ディスクキューに入っている状態だとしても、後続のクライアントのアクセスへは、メモリ内の最新の情報から応答されます）。
さらに、このようなインターフェイスの違いを考慮せず、純粋にアプリケーションのバックエンドとしてのデータベースにとって、データの（インデックスへの）同期が必要な場合と、必要でない場合とを考えることができます。RDBでの開発に慣れている開発者にとって、データベースの中で、データの同期が行われていない可能性を考えることは、違和感があるかもしれません。一方、RDBの開発経験が豊富な方ほど、インデックス定義によるデータ検索時の性能向上と、必然的に伴うデータ更新時の性能劣化との相克に悩まされた経験を持っているのではないかと思います。
まず、ここではインデックスは、あくまでデータの検索に使われることを確認しておきたいと思います。つまり、データがインデックスに同期されていない場合の影響は、あくまで検索結果の違いにのみ関係してきます。ある特定のレコード（ドキュメント）のカラム（フィールド）の値について、データベース内で同期が取れていない（特定時点で、異なる複数の結果を参照しうる）という現象は決して起こりません。そのことを考えれば、影響範囲は非常に限定的であることがわかると思います。また、ここでは非同期、つまり更新の遅延、を問題としていますが、クライアント（アプリケーション）から見た場合、この遅延は、究極的には、ネットワークその他、のあらゆる要因による遅延と区別できないと言えます、この後に触れる、ある１つのケースを除いては。


=== READ YOUR OWN DATA (自分自身のデータを読む)


複数のリクエストに対して矛盾した結果が返されたことを証明するためには、その矛盾の論拠として、正確な時刻情報を示す必要があります。これは、複数の異なるクライアントからのそれぞれ別の更新と検索リクエストを想定した場合、遅延が余程大きいものでなければ、非常に困難と言えるでしょう（上に触れたように、クライアントとデータベースの間には様々な遅延要因が存在しうると考えると、原因がデータベース内の遅延によるものだと証明することはシステムの内部情報に当たらない限り不可能だと言えます）。言い換えれば、複数のクライアント間の出来事と考えた場合、多少の遅延は事実上の問題となりえないと言えるでしょう。
唯一の例外は、同一のクライアントからのシーケンシャルなリクエストに対して矛盾した結果が返されるケースです。自分で行った更新が、その後の検索結果に反映されていないとしたら（例えばユーザー管理アプリケーション上で、ユーザーを追加した後に、その新規ユーザーが全ユーザーリストに表示されないとしたら）、これは明らかに問題です。


=== 永続化や複製の場合と比べた、インデックスの特殊性


データの永続化、複製作成、インデックス更新は、全て、データ更新とは非同期の処理であるという点で、共通しています。一方、データ永続化および複製作成については、データ更新時に、同期（処理が完了するのを待ってクライアントに成功のステータスが返されること）を強制するオプションが存在しています。これに対して、インデックス更新については、このようなデータ更新時のオプションはありません。


=== 一貫性を保証するためのオプション


データ更新時にインデックス更新の同期を強制するオプションが存在しない代わりに、N1QLクエリ実行時に、インデックス更新状況に対して、リクエストの挙動を変えることのできる複数のオプションがあります。



次のオプションを使用できます。

 * @<strong>{not_bounded}：クエリの一貫性を必要とせずに、クエリをすぐに実行します。インデックスの更新に遅延がある場合は、古い結果が返されることがあります。
 * @<strong>{at_plus}：インデックスが@<strong>{最新の更新のタイムスタンプ}まで更新されている事を保証します。インデックスの更新に遅延がある場合は、更新を待ちます。
 * @<strong>{request_plus}：インデックスが、@<strong>{クエリリクエストのタイムスタンプ}まで更新されている事を保証して、クエリを実行します。インデックスの更新状況に遅延がある場合は、更新を待ちます。



デフォルトの整合性は@<tt>{not_bounded}です。


上記のオプション表記は、概念を説明するためのものであり、SDK/プログラム言語によって実際の表記が変わることにご注意ください。以下は、Node.js SDK/JavaScriptでの上記オプションの利用例です。



//emlist[][js]{
const result = await cluster.query(
  "select * from `travel-sample`.inventory.airport", 
  {
    scanConsistency: couchbase.QueryScanConsistency.RequestPlus,
  });
//}

==== 参考情報


Couchbase公式ドキュメント Index Consistency@<fn>{b930f71470cfae6ed97d314e1717fbee}


//footnote[b930f71470cfae6ed97d314e1717fbee][https://docs.couchbase.com/java-sdk/current/concept-docs/n1ql-query.html#index-consistency]


== サンプルアプリケーション紹介

ここで紹介するサンプルアプリケーションを、下記で公開しています。

//blankline

@<href>{https://github.com/YoshiyukiKono/couchbase_step-by-step_node_jp}

//blankline

== CEANスタック

ここで、用いているCEANスタックとは、下記の技術要素からなります。

 * C: Couchbase Server (NoSQLドキュメント指向データベース)
 * E: Express (Webアプリケーション・フレームワーク）
 * A: Angular (フロントエンド・フレームワーク）
 * N: Node.js (サーバサイドJavaScript実行環境）

類似のものとして、MEANスタックという言葉を聞いたことがある方もいるのではないかと思います。この場合のMは、MongoDBを指します。MongoDBは、Couchbase同様、JSONデータを扱うNoSQLデータベースです。


== アプリケーション概要

サンプルアプリケーションは、下記の画面を見ていただきさえすれば、特にそれ以上の説明を必要としないであろう、最小限の機能からなるシンプルなものとなっています。


//image[sample_app_screen][sample app screen]{
  
//}

=== アプリケーション利用方法

==== Couchbase Server事前準備

このアプリケーションでは、バケット名として@<tt>{node_app}を使っています。バケットは事前に作成されている必要があります。


また、下記のインデックスを作成しておく必要があります。

//emlist[][shell]{
CREATE INDEX idx_node_app_user ON node_app.scp.user;
//}


==== コードの取得

上記のリポジトリから、コードを取得します。


//emlist[][shell]{
$ git clone https://github.com/YoshiyukiKono/couchbase_step-by-step_node_jp.git
$ cd couchbase_step-by-step_node_jp
//}

==== プログラム依存関係

その中に含まれる、@<tt>{package.json}には、下記の依存関係が定義されています。


//emlist[][json]{
  "dependencies": {
    "couchbase": "^3.1.0",
    "express": "^4.17.1"
  }
//}

==== 実行

このアプリケーションでは、実行の際に、下記のように、@<tt>{server.js}を使います。


//emlist[][shell]{
$ node server.js
Server up: http://localhost:80
//}


@<tt>{http://localhost:80}にアクセスします。


=== プログラム解説

==== ユーザーリスト表示


@<tt>{routes.js}を見ると、画面のリスト表示のために、下記のようなクエリが使われているのが分かります。


//emlist[][javascript]{
const qs = "SELECT name, id from node_app.scp.user"; 
//}

上記の(変数@<tt>{$qs}に代入された)をクエリ実行している箇所は以下の通りです。

//emlist[][javascript]{
const selectUsers = async (key) => { 
  const result = await cluster.query(qs, {
    scanConsistency: couchbase.QueryScanConsistency.RequestPlus,
  });
  return result.rows;
}
//}


@<tt>{script.js}中で、上記クエリのリクエストの戻り値は(@<tt>{response.data})は、下記のように、そのまま変数(@<tt>{$scope.users})に代入されます。、@<tt>{index.html}中で、画面表示に利用されます。


//emlist[][javascript]{
var getData = function() {
	return $http( {
		method: 'GET',
		url: '/users'
	}).then(function successCallback(response) {
		$scope.users = response.data;
		console.log('Success: ' + response);
	}, function errorCallback(response) {
		console.log('Error: ' + response);
	});
};
//}

代入された変数(@<tt>{$scope.users})は、@<tt>{index.html}で、画面表示に利用されます。

//emlist[][html]{
<tr ng-repeat="user in users">
	<td><input type="button" value="削除" data-ng-click="del_user(user)"></td>
	<td>{{user.id\}\}</td>
	<td>{{user.name\}\}</td>
</tr>
//}

==== 新規ユーザー追加

@<tt>{routes.js}中のデータ（ドメインオブジェクト）の表現と、そのデータをデータベースへの保存する部分は、下記のようなものです。


//emlist[][javascript]{
const user = {
  type: "user",
  id: req.body.id,
  name: req.body.name,
};
upsertDocument(user);
//}

JavaScriptのディクショナリをそのまま格納しているのが分かります。

//emlist[][javascript]{
const upsertDocument = async (doc) => {
  try {
    const key = `${doc.type}_${doc.id}`;
    const result = await collection.upsert(key, doc);
  } catch (error) {
    console.error(error);
  }
};
//}


==== ユーザー削除

ドキュメントの削除は、キー指定により行われます。

//emlist[][javascript]{
const removeUser = async (id) => {
  try {
    const key = `user_${id}`;
    const result = await collection.remove(key);
  } catch (error) {
    console.error(error);
  }
};
//}


== ODM(オブジェクトデータモデラー): Ottoman.js

Ottoman.js@<fn>{ottomanjs}は、Couchbase ServerとNode.jsのためのODM(オブジェクトデータモデラー)です。
Node.jsアプリケーション開発にCouchbase Serverを利用する際、Ottomanの利用は必ずしも必須ではありませんが、Ottomanは、開発者に様々な恩恵をもたらすことを目的として開発されています。
Ottomanは、MongoDBにおけるMongoose ODM@<fn>{mongoosejs}に相当するものであると言えます。


//footnote[ottomanjs][https://ottomanjs.com/]

//footnote[mongoosejs][https://mongoosejs.com/]
