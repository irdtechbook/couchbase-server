
= データ

== データ管理

ここでは、Couchbase Serverにおけるデータ管理について解説します。また、ユーザーにとっての選択肢を具体的に見ていきます。

=== ドキュメント

Couchbase Serverのデータ格納単位は、ドキュメントです。その構造を説明します。

==== ドキュメントキー（またはドキュメントID）

 * 可変長
 * 最大250バイト
 * 値は、バケット内において一意

==== メタデータ

 * 固定長
 * Couchbaseバケットに保存されているドキュメントの場合：56バイト
 * Ephemeralバケットに保存されているドキュメントの場合：72バイト

==== ドキュメント値

 * 可変長 (最大20MB)
 * 通常はJSONであることを想定しているが、バイナリデータの保存が可能

==== ドキュメント有効期限

ドキュメントには有効期限を設定することができます。

=== バケット

Couchbase Serverではドキュメントは、論理的コンテナ/キースペースとしてのバケットに保存されます。クラスターには、最大30のバケットを作成することができます。

//image[bucket-concept][bucket concept]{
//}

(画像は、 Couchbase Under the Hood: An Architectural Overview@<fn>{https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e}より引用)


====[column]
バケットに含まれるデータは、物理的には複数のノードに跨って管理されますが、ここでは、ユーザー/アプリケーショ ンからみた「論理的」なレベルのデータ管理について解説しています。

====[/column]

==== バケット種類

バケットにはCouchbaseバケットと、Ephemeralバケットの２種類があります（Couchbaseタイプがデフォルト）。

 * Couchbaseバケットは、ディスクに対して非同期でデータの永続化を行います。
 * Ephemeralバケットは、データに対する永続化の要件のない場合に用いられ、データはメモリ上にのみ存在します。


====[column]
CouchbaseServerのバケット種類には、Couchbaseバケット、Ephemeralバケットの他、Memcachedバケット があります。Memcachedバケットは、 Ephemeralバケットと同じ揮発性デー タ用のバケットとして、Ephemeralバ ケット登場以前から存在しています。現在では、Memcachedバケットは非 推奨(deprecated)とされています。

====[/column]

Couchbase Serverは、可能な限りすべてのデータ（アクティブ、およびレプリカ含め）をメモリに保持しようとします。

//image[document_conceptual_diagram][document conceptual diagram]{
//}

(画像は、 Couchbase Serverドキュメント Size Limits@<fn>{size-limits}より引用)

=== メモリからのデータ排出(Ejection)

メモリ内のデータがバケットの（ハイ・ウォーターマークと呼ばれる）メモリクォータの85％に達した場合、サーバーは、最近使用されていないものの一部をメモリから排出（削除）し始めます。データ排出プロセスは、メモリ内データがバケットメモリクォータの75％（ロー・ウォーターマークと呼ばれます）を下回るまで続きます。

//image[ejection_conceptual_diagram][ejection conceptual diagram]{
//}

(画像は、 Couchbase Serverドキュメント Ejection@<fn>{ejection}より引用)

==== 排出方法(Ejection Method)

排出方法として、Value-OnlyとFullの何かを選択可能です。

 * @<strong>{Value-only} 排出の際、値のみが排出され、キーとメタデータはメモリに残ります。より多くのメモリが必要ですが、最高のパフォーマンスを提供します。
 * @<strong>{Full} 排出の際、すべて（キー、メタデータ、値を含む）が排出されます。メモリ要件を軽減します。

==== 排出方法の選択

以下の二つのデータ保存要件の性格の違いによる、排出方法の選択を行うことが考えられます。

 * @<strong>{データ量は一定であることを想定（増加した過去データを別の場所へ退避する）} この場合には、事前の計画に基づいたリソース設計により、@<b>{Value-only}を選択することができ、最高のパフォーマンスを実現することができます。
 * @<strong>{データ量は経年的に増加することを想定} この場合、一般的なデータベースの場合、ディスクの容量に注目していれば十分だったとすると、データ数に応じて増えるメタデータがメモリ容量に与えるインパクトについても計画する必要があります。@<b>{Full}を選択することで、データ数（メタデータ量）増加によるインパクト、つまり一般的なデータベースの場合考慮する必要がなかった検討要素を回避することができます。

==== 排出方法の設定・反映

排出方法の設定は、バケット作成時に行うのみではなく、作成後の変更も可能です。
ただし、この変更は、（後から変更可能なバケット設定の中でも珍しく）、バケットのリスタート（設定変更時に暗黙に行われる）を必要とすることに注意が必要です。

=== 監視と警告

事前の計画が基本である一方、運用中の監視も重要な要素です。

==== 監視

以下は、WEB管理コンソール上の監視項目のイメージです（同じ内容をコマンドラインや、外部監視システムとの連携で利用することができます）。データ全体のサイズ（上）に加え、メタデータのサイズ（下）を監視することができkます。

//image[data_monitoring][data monitoring]{
//}

==== 警告

バケットのメモリクォータに対して占めるメタデータサイズの割合の超過に対する警告を利用することができます。

//image[data_alert][data alert]{
//}

=== ドキュメント有効期限

ドキュメントの有効期限は個々のドキュメント単位で設定することも、バケットレベルで設定することも可能です。両方に設定されている場合、より短い有効期限が採用されます。

====[column]
バケットレベルの有効期限設定は、エンタープライズエディションでのみ利用可能です。

====[/column]

==== 参考情報

Couchbaseブログ A Tale of Two Ejection Methods: Value-only vs. Full@<fn>{a-tale-of-two-ejection-methods-value-only-vs-full}

Couchbaseブログ The Best Database For Storing Images Might Not Be a Database At All@<fn>{the-best-database-for-storing-images-might-not-be-a-database-at-all}



//footnote[a-tale-of-two-ejection-methods-value-only-vs-full][https://blog.couchbase.com/a-tale-of-two-ejection-methods-value-only-vs-full/ 排出方法にフォーカスしたブログ記事]


//footnote[the-best-database-for-storing-images-might-not-be-a-database-at-all][https://blog.couchbase.com/the-best-database-for-storing-images-might-not-be-a-database-at-all/ データベース(Couchbase Server)利用の目的として、（一般によく取り上げられるものの中でも）必ずしも適切でないユースケースもあることを指摘しながら、Couchbase Serverのデータ管理、排出方法メカニズムとの関係を論じています。]




//footnote[size-limits][https://docs.couchbase.com/server/current/learn/data/data.html#size-limits]

//footnote[ejection][https://docs.couchbase.com/server/current/learn/buckets-memory-and-storage/memory.html#ejection]

//footnote[https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e][https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e]


== スコープとコレクション

=== 概要

Couchbase Serverは、ドキュメント管理のために、スコープとコレクションを提供します。これらを用いて、バケット内でドキュメントを分類・整理できます。

 * コレクションは、複数のドキュメントをグループ化するための機構です。
 * スコープは、複数のコレクションをグループ化するための機構です。

=== 存在意義

==== コレクションの存在意義

ドキュメントをコレクションによってグループ化することによって、ドキュメントの種類（ビジネスエンティティ）の違いを、コレクションによって表現することができます。
標準SQLをJSONデータへのアクセスのために拡張したクエリ言語であるN1QLクエリでは、コレクションは、リレーショナルデータベースにおけるテーブル相当の位置を占めることになります（具体的にいえば、FROM句で用いられます）。これによって、リレーショナルデータベースからCouchbase Serverへの移行が容易に実現されます（端的にいえば、データ構造を変更しない限り、同じクエリを用いることができます）。
念のため付け加えると、コレクションによって、その中に含まれるドキュメントの構造が一律のものとして規定されるわけではないため、JSONフォーマットの持つ柔軟性が損なわれることはありません。

==== スコープの存在意義

スコープによって、コレクションをグループ化することによって、ビジネスエンティティのグループを管理することができます。これによって、マルチテナンシーを実現することができます。Couchbase Serverは、スコープとコレクションによってアクセス権限を管理する機能を提供しています。

=== 機能

==== 適用範囲

スコープとコレクションによるデータ管理によって、Couchbase Serverの様々な機能において、以下が可能になります。

 * スコープとコレクションごとのアクセス権を割り当てることができます。これによって、各ユーザー/アプリケーションが必要なコレクションのみにアクセスできるようにできます。
 * コレクションにインデックスを付けるができます（スコープにインデックスを付けることはできません）。

また、Couchbase Serverの以下の機能において、スコープとコレクションが利用可能です。

 * N1QLクエリ
 * XDCR
 * バックアップ
 * 統計情報（cbstatsコマンド）


==== 基本仕様
 * クラスタごとに最大1000個のスコープを作成できます。
 * コレクション名は、スコープ内で一意である必要があります。
 * クラスタごとに最大1000のコレクションを作成できます。
 * アイテム名(ドキュメントのキー)は、コレクション内で一意である必要があります。


==== 作成・管理

スコープとコレクションは、以下を使用して作成・管理できます。

 * Web管理コンソール
 * CLI
 * REST API
 * N1QL
 * SDK


==== コレクションとドキュメント有効期限管理

コレクションに対して有効期限を設定することができます。これは、コレクション内のドキュメントの最大有効期限を決定します。
Couchbase Serverにおける有効期限の詳細については、ここでは触れません。

=== 種類と命名規則

==== デフォルトのスコープとコレクション

すべてのバケットには、デフォルトのスコープが自動的に与えられ、その中にはデフォルトのコレクションが与えられます。それぞれには、@<tt>{_default}という名前が付けられています。


特定のスコープまたはコレクションを指定せずに作成されたドキュメントは、デフォルトのスコープ内のデフォルトのコレクションに保存されます。
また、以下に注意する必要があります。

 * デフォルトのスコープは削除できません
 * デフォルトのコレクションは、CLIまたはREST APIのいずれかを使用して削除できます。 ただし、一旦削除すると、再作成できなくなります。

==== バージョン間の整合性

（スコープとコレクションが初めて導入された）7.0より前のバージョンのCouchbase Serverからアップグレードすると、既存のすべてのデータ（ドキュメント）がデフォルトのスコープとデフォルトのコレクションに自動的に配置されます。

==== ユーザー定義スコープ・コレクションの命名


ユーザー定義のスコープとコレクションには、ユーザー定義の名前を割り当てますが、以下を満たす必要があります。

 * 長さは1〜30文字
 * 大文字と小文字が区別されます
 * アルファベット(A-Z、a-z)、と数字(0-9)、記号として、@<tt>{_}, @<tt>{-}と@<tt>{%}のみを含めることができます（ただし、@<tt>{_}または@<tt>{%}で始めることはできません）

スコープ内の名前空間は、他のスコープの名前空間から独立しています。同一バケット内の複数のスコープに同じコレクション名が存在することができます。同様に、コレクション内の名前空間は、他のコレクションの名前空間から独立しています。その結果、あるバケット内の同じ(または、異なる)スコープの複数のコレクションに、同じドキュメントキーが存在することができます。




==== 参考情報


Couchbase公式ドキュメント Scope and Collection@<fn>{a55c3a0e13fc128db7ae95f8a1953f19}


//footnote[a55c3a0e13fc128db7ae95f8a1953f19][https://docs.couchbase.com/server/current/learn/data/scopes-and-collections.html]
