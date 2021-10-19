
= Node.jsアプリケーション開発

== なぜ、Node.jsとCouchbase Serverの組み合わせなのか


JSONデータは、JavaScriptと高い親和性を持っています。
Couchbase Serverを用いることによって、JSONデータとクエリ言語の両方の利点を活用することができます。


=== JSONデータの利点

まず、アプリケーションのデータ層にリレーショナルデータベースを用いる場合に、問題となりうる点を以下に挙げます。

//blankline

 * @<strong>{第一正規形テーブル構造}と、アプリケーションが扱うデータモデル（ドメインオブジェクト）との間にギャップがありうるため、データ層とアプリケーションの間で、データ構造を変換する必要が生じる場合がある。
 * @<strong>{厳格なテーブルスキーマ}に基づくデータ管理は、アプリケーション設計・開発の柔軟さを損なう場合がある。

//blankline

これに対して、データ層がJSONデータを許容した場合、以下の利点を得ることができます。

//blankline

 * データは第一正規形であることが要請されないため、アプリケーションが扱うデータモデル（ドメインオブジェクト）をそのまま格納することができる。
 * アプリケーション設計・開発において、データモデルの変化に柔軟に対応できる。


=== クエリ言語の利点

標準化されたクエリ言語であるSQLが様々な異なるデータベースで利用できることは、リレーショナルデータベースの繁栄の大きな要因であり、SQLの習得はアプリケーション開発者にとって、必修とさえいえるものとなっていきました。

Couchbase Serverを選択することで、開発者は既存のSQLの知識・経験を活用することができます。

== キーバリュー操作

Couchbase Serverでは、開発者はクエリを利用するだけでなく、キー指定によるデータ(ドキュメント)の操作を行うことが可能です。
これをキーバリュー操作(Key Value Operations)と呼びます。


ここでは、アプリケーション開発におけるキーバリュー操作について、プログラミング例を用いて紹介します。詳細についてはドキュメント@<fn>{kv-operations}を参照してください。

=== CRUD操作

キーバリュー操作の基本となるCRUD操作に関して、以下のようなメソッドが利用可能です。

//blankline

@<strong>{生成}(@<tt>{CREATE})

//emlist[][js]{
const result = await collection.insert(key, document);
//}

@<strong>{読み取り}(@<tt>{READ})

//emlist[][js]{
const result = await collection.get(key);
const document = result.value;
//}

@<strong>{更新}(@<tt>{UPDATE})


//emlist[][js]{
const result = await collection.replace(key, document);
//}


@<strong>{削除}(@<tt>{DELETE})

//emlist[][js]{
const result = await collection.remove(key);
//}

@<strong>{生成または更新}(@<tt>{UPSERT})


//emlist[][js]{
const result = await collection.upsert(key, document);
//}


@<tt>{UPSERT}メソッドは、同一のキーを持つデータが存在しなければ、生成（@<tt>{CREATE/INSERT}）を行い、既に同一のキーを持つデータが存在している場合は更新（@<tt>{UPDATE/REPLACE}）を行います。


//footnote[kv-operations][https://docs.couchbase.com/nodejs-sdk/current/howtos/kv-operations.html]

=== サブドキュメント操作

データを参照する際に、ドキュメント全体ではなく、一部を参照すれば十分な場合があります。
また、データの更新を行う場合、ドキュメントを取得し、必要な箇所を編集した上で、同じキーで保存するのは単純で分かりやすい操作ですが、ドキュメントの指定箇所の更新を指示することができれば十分な場合があります。
このような操作はN1QLを利用しても行うことができますが、キーバリュー操作でも実行することができます。これをサブドキュメント操作と呼びます。

サブドキュメント操作の基本的な種類を、プログラミング例を用いて紹介します。

//blankline

@<strong>{生成}(@<tt>{CREATE})

//emlist[][js]{
await collection.mutateIn(key, [
  couchbase.MutateInSpec.insert("purchases.complete", [42, true, "None"]),
]);
//}

@<strong>{読み取り}(@<tt>{READ})

//emlist[][js]{
const result = await collection.lookupIn(key, [
  couchbase.LookupInSpec.get("addresses.delivery.country"),
]);
const country = result.content[0].value;
//}


@<strong>{更新}(@<tt>{UPDATE})

//emlist[][js]{
await collection.mutateIn(key, [
  couchbase.MutateInSpec.replace("email", "xyz@example.com"),
]);
//}


@<strong>{削除}(@<tt>{DELETE})

//emlist[][js]{
await collection.mutateIn(key, [
  couchbase.MutateInSpec.remove("addresses.billing"),
]);
//}

@<strong>{生成または更新}(@<tt>{UPSERT})

//emlist[][js]{
await collection.mutateIn(key, [
  couchbase.MutateInSpec.upsert("fax", "123-456-7890"),
]);
//}

@<tt>{UPSERT}メソッドは、指定したフィールドが存在しなければ追加し、既にフィールドが存在している場合はその値を更新します。

//blankline

@<strong>{存在確認}


//emlist[][js]{
const result = await collection.lookupIn(key, [
  couchbase.LookupInSpec.exists("purchases.pending[-1]"),
]);
console.log("Path exists? ", result.content[0].value);
//} 

データを取得するのではなく、指定したパスが存在するかどうかを確認することが可能です。

//blankline

@<strong>{複数操作の同時リクエスト}

上掲のコードを見ればわかりますが、サブドキュメント操作は配列型の引数で与えられています。これまでの例では要素がひとつの配列が使われていますが、下記のように複数の操作を一度に与えることが可能です。

//emlist[][js]{
const result = await collection.lookupIn(key, [
  couchbase.LookupInSpec.get("addresses.delivery.country"),
  couchbase.LookupInSpec.exists("purchases.pending[-1]"),
]);

console.log(result.content[0].value); 
console.log("Path exists?", result.content[1].value);
//}

//blankline

@<strong>{その他の操作}

ここではCRUD操作を中心に紹介しましたが、Couchbase Serverのサブドキュメント操作の範囲は上記にとどまるものではありません。

たとえば、JSONドキュメントは値を配列として持つことができますが、配列に対する値の追加・挿入を実行することが可能です。

また、値を数値として持つフィールドに対して、一旦値を取得した後に、増減した値で更新するのではなく、直接値の変更幅を指定するカウンター操作も用意されています。

その他、詳細についてはドキュメント@<fn>{subdocument-operations}を参照してください。

//footnote[subdocument-operations][https://docs.couchbase.com/nodejs-sdk/current/howtos/subdocument-operations.html]


====[column]バルク操作API

ここまで、Couchbase Serverのキーバリュー操作について見てきました。読者の中には、サブドキュメント操作については複数の操作を一括して(バルクで)リクエストするAPIについて解説しているのに対して、ドキュメントに関するCRUD操作では、そのようなバルク操作について触れられていない点に注目された人がいるかもしれません。

この違いは、意図的な説明の省略によるものではなく、Couchbase ServerのSDKに由来するものです。

Couchbase Server SDKのAPIには、同期型と非同期型の２種類が提供されています。
そして、同期型のAPIの内部では非同期型APIが用いられ、結果が戻される前に内部的に処理がブロック(同期)されることで、非同期処理が隠されています。
非同期プログラミングについては、同期的なプログラミングとは異なる知識が必要であり、十分に理解せずに用いることには弊害が伴います。そのため、同期型のラッパーAPIが提供されているのは理由があることだといえます。

ここにバルク処理のAPIを付け加えることは、本来の処理を二重に隠すことにつながります。
非同期APIの恩恵は、前後の処理を含めて可能な限り非同期で実行することによって得られます。そして、非同期で処理が可能な範囲は、アプリケーション毎に異なります。

しかしながら、このような疑問を持たれることは珍しいことではないらしく、コミュニティーフォーラムでも、ひとつのスレッド(How to do bulk get in 2.x java sdk@<fn>{how-to-do-bulk-get-in-2-x-java-sdk})が立てられています。このスレッドには、有識者による回答として、非同期APIを内部的に用いたバルク処理メソッドの実装も示されています(Node.jsではなく、Java SDKによる実装ですが)。

アプリケーションの要件を考慮せず、一律に同期を行う戦略は、必ずしも最善の選択であるとはいえず、だからこそSDKは単純なラッパーAPIを提供していないともいえます。ですが、ここで挙げた論点に関する議論と、それに対するひとつの解決策のサンプルとして興味深いと考えられたため、紹介します。興味のある方は、スレッドを参照してみてください。

====[/column]



//footnote[how-to-do-bulk-get-in-2-x-java-sdk][https://forums.couchbase.com/t/how-to-do-bulk-get-in-2-x-java-sdk/4634/5]


== データ一貫性(Consistency)

はじめに、アプリケーションに必要なデータ一貫性(Consistency)を実現するための論点を整理します。その後、関連するCouchbase Serverの機能を解説します。

=== 分散アーキテクチャーにおけるインデックス

Couchbase Serverの特徴として、メモリーファーストアーキテクチャーがあります。
このアーキテクチャーにより、永続化装置およびネットワークの性能が、データベースの性能のボトルネックになることが回避されます。
ディスクへの書き込みは、キューを介して行われます。また、他ノードへの複製（レプリカ）のためのキューメカニズムとして、Data Change Protocol(DCP)が存在します。

DCPはレプリカだけではなく、データの変更を他のサービスに反映するために広く利用されます。データ更新時のインデックスへの反映もそのひとつです。


=== インデックス更新から見たRDBとの違い


データ更新時に、関連するインデックスの更新が行われるのは、Couchbase ServerとRDBとで共通です。

RDBでは、クライアントからのデータ更新処理リクエストに対して、成功のステータスが返されるのは、インデックス更新の終了後です。これは、データ一貫性の保証という要件面に加え、RDBがモノリシックなアークテクチャーからなっているという技術面から見ても自然な挙動といえます。

一方で、Couchbase Serverは分散アーキテクチャーという性格を持ち、データの更新処理を司るDataサービスとインデックスを管理するIndexサービスとは、コンポーネントとして独立しています。また、本番環境では、それらのサービスは別々のサーバーに配置されるのが一般的です。Couchbase Servevrでは、インデックスの更新は、永続化装置への反映や複製の作成と同様、キュー（非同期）のメカニズムで実現されています。


=== キーバリュー操作とクエリ

RDBにとって、SQLがクライアントにとって共通のインターフェースであるのと異なり、Couchbase Serverでは、N1QLはクライアントにとって選択することのできる複数のインターフェースのうちのひとつです。

さらにいえば、Dataサービスへのキーバリュー操作が、Couchbase Serverにおける一次的なインターフェイスであるのに対して、N1QLクエリはあくまで派生的なインターフェースであるといえます。
キーによるアクセスでは、常にデータの同期が保証されます（たとえ、更新が永続化されておらず、ディスクキューに残っている状態だとしても、クライアントからのリクエストへの応答には、メモリー内の最新の情報が用いられます）。

N1QLクエリは、インデックスに依存するため、デフォルトではクエリの結果が最新のデータと同期されていることが保証されません(同期を保証する方法について後に触れます)。


=== インデックス更新が非同期であることの影響範囲

アプリケーションのバックエンドとしてのデータベースにとって、データ更新とインデックス更新の同期が必要な場合と、必要でない場合とを考えてみることができます。

RDBでの開発に慣れている開発者にとって、データベースの中でデータとインデックスの同期が取れていない可能性を考えることは、違和感があるかもしれません。
一方、経験が豊富な開発者ほど、インデックス定義によるデータ検索時の性能向上と、必然的に伴うデータ更新時の性能劣化との相克に悩まされた経験を持っているのではないかと思います。

まず、インデックスはあくまでデータの「検索」に使われることを確認しておきたいと思います。つまり、データの変更がインデックスに同期されていない場合の影響は、あくまで検索結果の違いにのみ関係してきます。ある特定のレコード（ドキュメント）のカラム（フィールド）の値について、データベース内で同期が取れていない（特定時点で、異なる複数の結果を参照しうる）という現象は起こりません。
ここから見ても、インデックスの更新が非同期であることの影響範囲は限定的であることがわかります。

また、ここでは非同期、つまり更新の遅延を問題としていますが、クライアント（アプリケーション）から見た場合、この遅延は究極的には、ネットワークその他のあらゆる要因による遅延と区別できないともいえます。この後に触れる、あるひとつのケースを除いては。


=== Read Your Own Writes (自分自身が書いたデータを読む)


複数のリクエストに対して矛盾した結果が返されたことを証明するためには、その矛盾の論拠として、正確な時刻情報を示す必要があります。これは、複数の異なるクライアントからのリクエストを想定した場合、遅延が余程大きいものでなければ、非常に困難といえるでしょう（上に触れたように、クライアントとデータベースの間には様々な遅延要因が存在しうるため、原因がデータベース内の遅延によるものだと証明することはシステムの内部情報に当たらない限り不可能だといえます）。言い換えれば、複数のクライアント間の出来事と考えた場合、多少の遅延は事実上の問題となりえないといえるでしょう(ここでは、あくまで複数のクライアントが「継続的に」アプリケーションを利用し続けている状況について語っています。これは、システムが利用されているほとんどの時間を占めている、といえます。しかしながら、より頻繁ではないにしろ、「特定時点のデータ断面」の把握が必要になる要件は、当然あり得るでしょう。特定時点までのデータ同期を保証する方法については、後に触れます)。

Webアプリケーションでは、複数のクライアントからの複数のリクエストと、同一のクライアントからの複数のリクエストが、さほど明確に区別されない場合があります。
だからこそ、同一のクライアントからのシーケンシャルなリクエストとして「Read Your Own Writes (自分自身が書いたデータを読む)」ケースを、特別なものとして考慮する必要があります。
自分で行った更新が、その後の検索結果に反映されないとしたら、これは明らかに問題です。


=== 永続化や複製の場合と比べたインデックスの特殊性

データの永続化、複製、インデックス更新は、全て、データ更新とは非同期の処理であるという点で共通しています。一方、データ永続化および複製については、データ更新時に、同期（処理が完了するのを待ってクライアントに成功のステータスが返されること）を強制する永続性(Durability@<fn>{data-durability})オプションが存在しています。これに対して、インデックス更新については、このようなデータ更新時のオプションはありません(この違いは、永続化や複製がデータ更新と同じく、Dataサービスの役割であることを考えると、自然と納得されます)。

//footnote[data-durability][https://docs.couchbase.com/server/current/learn/data/durability.html]

=== 一貫性を保証するためのオプション


データ更新時にインデックス更新の同期を強制するオプションが存在しない代わりに、N1QLクエリ実行時における、インデックスの一貫性(Index Consistency@<fn>{b930f71470cfae6ed97d314e1717fbee})に関する選択肢があります。

この選択肢は、以下のようなキーワードを用いて区別されます。

//blankline

 * @<strong>{NOT_BOUNDED}: クエリの一貫性を必要とせずに、クエリをすぐに実行します。インデックスの更新に遅延がある場合は、古い結果が返されることがあります。
 * @<strong>{REQUEST_PLUS}: インデックスが、@<strong>{クエリリクエストのタイムスタンプ}まで更新されるのを待って、クエリを実行します。
 * @<strong>{AT_PLUS}: インデックスが@<strong>{クライアントの直近の更新のタイムスタンプ}まで更新されるのを待って、クエリを実行します。

//blankline

 はじめのふたつについては、SDKによるクエリ実行時に、直接指定できるオプションがあり、オプションを指定しない場合のデフォルトは@<tt>{NOT_BOUNDED}です。

上記のオプション表記は、概念を説明するためのものであり、SDK/プログラム言語によって実際の表記が変わることにご注意ください。以下は、Node.js SDKでの@<tt>{REQUEST_PLUS}オプションの利用例です。

//emlist[][js]{
const result = await cluster.query(
  "select * from `travel-sample`.inventory.airport", 
  {
    scanConsistency: couchbase.QueryScanConsistency.RequestPlus,
  });
//}

@<tt>{REQUEST_PLUS}が、リクエストのタイムスタンプを基準にしているため、クライアント/アプリケーションの状態に依存していないのに対して、@<tt>{AT_PLUS}は本来の意味での「Read Your Own Writes」を実現します。@<tt>{AT_PLUS}の実現には、データ更新時のトークンの取得が必要になります。取得したトークンをクエリ実行時に指定することによって、アプリケーションに必要十分な一貫性を、最低限の負荷により保証することができます。

====[column]本書におけるトークンの扱い、および参考情報

本書では、トークンの利用については扱いません。

代わりに、サンプルコードが掲載されているドキュメント@<fn>{nodejs-sdk-2_6-scan-consistency-examples}を示します。

概略を述べると、トークンを利用するにはトークンの取得を有効化した上で、データ更新時のレスポンスから@<tt>{MutationState}オブジェクトを取得し、クエリ実行時に、@<tt>{MutationState}オブジェクトを引数に取るメソッド(@<tt>{consistentWith})を利用します。

====[/column]

//blankline

====[column]サンプルコード実装に関する注記

本番システムを考えると、リソース消費の面で悪手であることは間違いありませんが、@<tt>{REQUEST_PLUS}を用いても、「Read Your Own Writes」のケースに対応することは可能です。
本書のサンプルコードでは、こちらの方法を用いています。

付け加えると、リソース消費の最適化のためには、必要な時にのみ@<tt>{AT_PLUS}、ないし@<tt>{REQUEST_PLUS}を用いるべきであり、必要な要件のない限り、@<tt>{NOT_BOUNDED}を用いるべきです。
その観点から、同じ検索処理であっても、データの更新後とそれ以外の通常の場合とでは、オプションを変更するという実装を行うことが考えられます(本書のサンプルコードでは、区別なしに@<tt>{REQUEST_PLUS}を用いています)。


====[/column]

//footnote[b930f71470cfae6ed97d314e1717fbee][https://docs.couchbase.com/java-sdk/current/concept-docs/n1ql-query.html#index-consistency]

//footnote[nodejs-sdk-2_6-scan-consistency-examples][https://docs.couchbase.com/nodejs-sdk/2.6/scan-consistency-examples.html]

//footnote[java-sdk-2_7-scan-consistency-examples][https://docs.couchbase.com/java-sdk/2.7/scan-consistency-examples.html]


//footnote[node-sdk-current-n1ql-query-index-consistency][https://docs.couchbase.com/nodejs-sdk/current/concept-docs/n1ql-query.html#index-consistency]

== サンプルアプリケーション紹介

ここで紹介するサンプルアプリケーションを、下記で公開しています。

//blankline

@<href>{https://github.com/YoshiyukiKono/Couchbase_Server_First_Step_Guide.git}

//blankline

=== CEANスタック

ここで、用いているCEANスタックとは、下記の技術要素からなります。

//blankline

 * C: Couchbase Server (NoSQLドキュメント指向データベース)
 * E: Express@<fn>{expressjs} (Webアプリケーションフレームワーク）
 * A: Angular@<fn>{angular} (フロントエンドフレームワーク）
 * N: Node.js@<fn>{nodejs} (サーバーサイドJavaScript実行環境）

//blankline

類似のものとして、MEANスタックという言葉を聞いたことがある方もいるのではないかと思います。その場合のMは、MongoDBを指します。

//footnote[expressjs][http://expressjs.com/]

//footnote[angular][https://angular.io/]

//footnote[nodejs][https://nodejs.org/]

=== アプリケーション概要

サンプルアプリケーションは、最小限の機能からなるシンプルなものとなっています。下記の画面を見ていただきさえすれば、特にそれ以上の説明は必要ないでしょう。


//image[sample_app_screen][サンプルアプリケーション画面]{
  
//}

=== アプリケーション実行事前準備


アプリケーションのデータ管理のために、以下の構成を前提としています。これらは事前に作成されている必要があります。

//blankline

 * バケット: @<tt>{user_management}
 * スコープ: @<tt>{japan}
 * コレクション: @<tt>{users}

//blankline

また、下記のインデックスを作成しておく必要があります。このサンプルアプリケーションでは、ユーザーのリストを表示する際に、検索条件を用いないクエリを利用しているためプライマリインデックスを作成しています。

//emlist[][sql]{
CREATE PRIMARY INDEX primary_idx_users ON user_management.japan.users
//}

アプリケーションがCouchbase Serverにアクセスするために利用するアカウントを以下の内容で、新規作成します。

//blankline

 * ユーザー: @<tt>{user_management_app}
 * パスワード: @<tt>{C0uchb@se}
 * アクセス権: 上記バケット(@<tt>{user_management})に対する@<tt>{Application Access}の権利を追加

=== アプリケーション利用方法

GitHubリポジトリーをクローンし、作成されたディレクトリーへ移動します。


//cmd{
$ git clone https://github.com/YoshiyukiKono/Couchbase_Server_First_Step_Guide.git
$ cd Couchbase_Server_First_Step_Guide/
//}

ディレクトリーの中に含まれる、@<tt>{package.json}には、下記の依存関係が定義されています。

//emlist[][json]{
  "dependencies": {
    "couchbase": "^3.2.1",
    "express": "^4.17.1"
  }
//}

アプリケーションが依存するパッケージをプロジェクトにインストールします。

//cmd{
$ npm install
//}


アプリケーションを実行します。下記のように、@<tt>{server.js}を使います(または、@<tt>{npm start}に置き換えることもできます)。


//cmd{
$ node server.js
Server up: http://localhost:80
//}


@<tt>{http://localhost:80}にアクセスします。


=== プログラム解説

@<strong>{Couchbase Serverへの接続}


Couchbase Serverは、アプリケーションと同じ環境で実行されていることが想定されています。

//emlist[][javascript]{
const cluster = new couchbase.Cluster('couchbase://localhost', {
    username: 'user_management_app',
    password: 'C0uchb@se',
})
//}

@<strong>{ユーザーリスト表示}


@<tt>{routes.js}を見ると、ユーザーのリスト表示のために下記のクエリが使われているのがわかります。


//emlist[][javascript]{
const qs = 'SELECT id, name from users'; 
//}

上記の(変数@<tt>{qs}に代入された)クエリを実行している箇所は以下の通りです。

//emlist[][javascript]{
const selectUsers = async (key) => {
  const result = await scope.query(qs, {
		scanConsistency: couchbase.QueryScanConsistency.RequestPlus,
	  });
  return result.rows;
}
//}


@<tt>{script.js}の中で、上記クエリのリクエストの戻り値(@<tt>{response.data})は、そのまま変数(@<tt>{$scope.users})に代入されます。


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

@<tt>{index.html}の中で、代入された変数(@<tt>{users})は、下記のように画面表示に利用されます。

//emlist[][html]{
<tr ng-repeat="user in users">
<td><input type="button" value="削除" data-ng-click="del_user(user)"></td>
<td>{{user.id}}</td>
<td>{{user.name}}</td>
</tr>
//}

@<strong>{新規ユーザー追加}


@<tt>{routes.js}の中のデータ（ドメインオブジェクト）の表現と、そのデータをデータベースへ保存する部分は以下の通りです。


//emlist[][javascript]{
const user = {
  id: req.body.id,
  name: req.body.name,
};
upsertDocument(user);
//}

以下の箇所を見ると、JavaScriptのディクショナリがそのままドキュメントとして格納されているのがわかります。

なお、既存のIDでユーザーを追加した場合には、新しい情報で上書きされる仕様としています。

//emlist[][javascript]{
const addUser = async (doc) => {
  try {
    const key = `${doc.id}`;
    const result = await collection.upsert(key, doc);
  } catch (error) {
    console.error(error);
  }
};
//}


@<strong>{ユーザー削除}


ドキュメントの削除は、キー指定により行われます。

//emlist[][javascript]{
const removeUser = async (id) => {
  try {
    const key = `${id}`;
    const result = await collection.remove(key);
  } catch (error) {
    console.error(error);
  }
};
//}

== Node.js SDKを開発に利用する


新しく作成するNode.jsアプリケーションで、Couchbase ServerのNode.js SDKを利用するための手順を記します。

=== 動作確認環境

 * Mac OS Catalina Version 10.15.6
 * Node.js v15.4.0
 * npm 7.0.15

=== プロジェクト準備手順

プロジェクト用のディレクトリーを作成し、その中で作業を開始します。

はじめにプロジェクトを初期化します。

//cmd{
$ npm init -y
//}

package.jsonファイルが作成されます。

次に、Node.js SDKをインストールし、package.jsonに追記します。

//cmd{
$ npm install couchbase --save
//}

package.jsonに以下のような依存関係が追記されます。

//emlist[][json]{
  "dependencies": {
    "couchbase": "^3.2.1"
  }
//}

== ODMフレームワーク Ottoman.js

=== Ottoman.jsとは

Ottoman.js@<fn>{ottomanjs}は、Couchbase ServerとNode.jsのためのODM(オブジェクトデータモデラー)です。

Node.jsアプリケーション開発にCouchbase Serverを利用する際、Ottomanの利用は必須ではありませんが、Ottomanは、開発者に様々な恩恵をもたらすことを目的として開発されています。@<fn>{ottoman-name}

Ottomanは、MongoDBにおけるMongoose ODM@<fn>{mongoosejs}に相当するものであるといえます。


//footnote[ottomanjs][https://ottomanjs.com/]


//footnote[ottoman-name][Ottoman(オットマン)の元々の意味は、椅子やソファーの前において、足を乗せるために使う小型のソファーのことです。フットスツールとも呼ばれます。名前は、発祥地であるオスマン帝国に由来します。Ottoman.jsの命名については、カウチ(ソファー)と組み合わせて利用するものであることから来ているのでしょう。]

//footnote[mongoosejs][https://mongoosejs.com/]

=== 機能概要

Couchbase Serverでは、RDBのようにデータに対してスキーマが強制されることはありません。
しかしながら、アプリケーション開発において、スキーマは依然として重要な役割を持ちます。

Ottomanは、アプリケーションサイドにおけるスキーマによるデータバリデーション(検証)のフレームワークを提供します。

また、OttomanはCouchbase Server SDKに対する抽象化レイヤーを提供します。
Ottomanライブラリーが使われるとき、Ottomanを介して、透過的にCouchbase Serverとのデータのやり取りが行われます。


====[column]Ottoman関連情報

Ottomanについては、当初本書では扱うことを考えていませんでしたが、MongoDBのMongoose ODM(の存在)を知っている人が持つかもしれない疑問/興味に応えるために、紹介を目的として、ごく基本的な情報のみ記しています。

ここでは、Ottomanに関心を持たれた方向けに、追加の情報を提供します。

Ottomanは、2021年9月にv2.0がリリースされています。

公式サイトには、クィックスタート(Quick Start with Ottoman v2)が公開されており、利用を始める際の参考にすることができます。

また、v2.0のリリースにあわせて、ブログ記事(「Introducing Ottoman v2.0: An ODM for Node.js & Couchbase」@<fn>{ottoman-2-0-odm-node-js-couchbase})が発表されています。
この記事はv2.0の新規機能のみではなく、Ottomanそのものへのイントロダクションとなっているため、Ottomanを使って、何ができるのかを理解するために有用だと思われます。

また、サンプルアプリケーション@<fn>{try-ottoman}も公開されており、v2.0リリースに合わせてアップデートされています。


====[/column]

//footnote[quick-start-with-ottoman][https://ottomanjs.com/guides/quick-start.html#quick-start-with-ottoman-v2]

//footnote[ottoman-2-0-odm-node-js-couchbase][https://blog.couchbase.com/ottoman-2-0-odm-node-js-couchbase/]
//footnote[try-ottoman][https://github.com/couchbaselabs/try-ottoman]


== 開発の実践に向けて

本書によって、Couchbase Serverを使った開発について、基本的なイメージを掴むことができたと感じていただけたのなら幸いです。

一方、当然のことながら、アプリケーション開発は一冊の書籍で語り尽くせてしまうほど単純なものではなく、困難を伴うが故に奥深く、興味深いものです。

ここでは、さらに学習を進めるにあたって有益と思われる情報を紹介し、締め括りとしたいと思います。

=== コミュニティー

Couchbase Dev Communityサイト@<fn>{developer-couchbase}では、Couchbase開発者コミュニティーへ向けて、チュートリアルやベストプラクティス等、様々な情報提供が行われています。
また、フォーラム@<fn>{forums-couchbase}で、Couchbase Serverコミュニティーエディションに関する疑問について、過去に行われた質疑応答を検索したり、質問をすることができます。

=== Couchbase Labsとサンプルアプリケーション

Couchbase Labs GitHub@<fn>{github-couchbaselabs}では、Couchbaseに関連する様々なプロジェクトが公開されています。

Ottomanのようなフレームワークの他、サンプルアプリケーションも公開されています。
たとえば、travel-sampleサンプルバケットを使ったアプリケーションが公開されており、フロントエンドは、Node.jsで実装されています。@<fn>{try-cb-frontend}
そのフロントエンドと組み合わせて利用することができるバックエンドのREST APIアプリケーションは、各種プログラミング言語での実装が公開されており、Node.js SDKによる実装も存在します。@<fn>{try-cb-nodejs}


=== 無償オンライントレーニング

Couchbase, Inc.が運営するCouchbase Academy@<fn>{couchbase-academy}では、各種のトレーニングコースや認証資格が提供されています。
自分のペースで受講することができる、Node.js SDKについての無償のオンライントレーニングとして、「CB130n: Couchbase Associate Node.js Developer Certification Course@<fn>{node-js-developer-certification-course}」が利用できます(対応する認証資格試験の受講は有償になります)。

=== ドキュメント、API Docs、ソースコード

ドキュメント@<fn>{nodejs-sdk-overview}とAPI Docs@<fn>{nodejs-sdk-api-modules}は、常に開発者のよき友です。
さらにお望みとあらば、ソースコード@<fn>{github-couchbase-couchnode}を参照することもできます。


//footnote[github-couchbaselabs][https://github.com/couchbaselabs/]

//footnote[try-cb-nodejs][https://github.com/couchbaselabs/try-cb-nodejs]
//footnote[try-cb-frontend][https://github.com/couchbaselabs/try-cb-frontend-v2]

//footnote[developer-couchbase][https://developer.couchbase.com/]

//footnote[forums-couchbase][https://forums.couchbase.com/]

//footnote[couchbase-academy][https://www.couchbase.com/academy]
//footnote[node-js-developer-certification-course][https://learn.couchbase.com/store/1246860-cb130n-couchbase-associate-node-js-developer-certification-course]

//footnote[nodejs-sdk-overview][https://docs.couchbase.com/nodejs-sdk/current/hello-world/overview.html]

//footnote[nodejs-sdk-api-modules][https://docs.couchbase.com/sdk-api/couchbase-node-client/modules.html]

//footnote[github-couchbase-couchnode][https://github.com/couchbase/couchnode]