
= データ


ここでは、クライアント/アプリケーションから見たデータについて解説します。
Couchbase Serverでは、データは複数のノードに跨って管理されますが、データを利用する場合、表面的にはそのことを意識する必要はありません。

== ドキュメント

=== 概要

Couchbase Serverにおける、データ格納の最小単位はドキュメントです。

ドキュメントは、以下の構成要素からなります。概念図とあわせてご確認ください。

//blankline

 * キー
 * 値
 * メタデータ
 * ユーザー拡張属性
 * システム拡張属性

//image[document_conceptual_diagram][ドキュメント構成要素概念図]{
//}



//footnote[size-limits][https://docs.couchbase.com/server/current/learn/data/data.html#size-limits]

(画像は、 Couchbase Serverドキュメント Size Limits@<fn>{size-limits}より引用)

//blankline


以下、ドキュメントを構成する要素について、基本的な仕様を整理します(拡張属性については割愛します)。

=== キー

 * 可変長(最大250バイト)
 * キースペース内において一意
 * スペースを含まないUTF-8文字列

=== 値

 * 可変長(最大20MB)
 * 値をJSON(RFC 8259@<fn>{rfc8259}に準拠)として持つことによって、クエリ等のドキュメント指向データベースとしての各種機能を活用することができるが、バイナリデータを保存してKVSとしての機能範囲(キーによるアクセスのみ)で利用することも可能

//footnote[rfc8259][https://datatracker.ietf.org/doc/html/rfc8259]

=== メタデータ

 * 固定長
 * Couchbaseバケットの場合: 56バイト
 * Ephemeralバケットの場合: 72バイト


== バケット

=== 概要

ドキュメントは、論理的コンテナとしてのバケットに保存されます。

クラスターには、最大30のバケットを作成することが可能です。


//image[bucket-concept][論理的コンテナとしてのバケット]{
//}

(画像は、 Couchbase Under the Hood: An Architectural Overview@<fn>{server-arc-overview}より引用)

//blankline

//footnote[server-arc-overview][https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e]

=== 種類

バケットにはCouchbaseバケットと、Ephemeralバケットの２種類があります（Couchbaseタイプがデフォルト）。

//blankline

 * @<strong>{Couchbaseバケット}は、非同期でデータの永続化を行います(同期的な永続化を指示することも可能)。可能な限り多くのデータをメモリーに保持しようとします。
 * @<strong>{Ephemeralバケット}は、データに対する永続化の要件のない場合に用いられ、データはメモリー上にのみ存在します。


====[column]Memcachedバケット

Couchbase Serverのバケットの種類には、Couchbaseバケット、Ephemeralバケットの他、Memcachedバケットがあります。
Memcachedバケットは、 Ephemeralバケットと同じ揮発性データ用のバケットとして、Ephemeralバケット登場以前から存在しています。
現在では、Memcachedバケットの利用は非推奨(deprecated)とされています。

====[/column]



== スコープとコレクション

=== 概要

Couchbase Serverは、ドキュメント管理のために、スコープとコレクション(Scopes and Collections@<fn>{a55c3a0e13fc128db7ae95f8a1953f19})を提供します。
これらを用いて、バケット内でドキュメントを分類し整理することができます。

//blankline

 * コレクションは、ドキュメントをグループ化します。
 * スコープは、コレクションをグループ化します。



=== コレクションの存在意義

ドキュメントをコレクションによってグループ化することによって、ドキュメントの種類（エンティティー）を表現することができます。

コレクションは、リレーショナルデータベースにおけるテーブル相当の位置を占めることになります。

コレクションによって、リレーショナルデータベースからCouchbase Serverへの移行が容易になります。

標準SQLをJSONデータへのアクセスのために拡張したクエリ言語であるN1QLクエリでは、FROM句にコレクション名を用います。RDBからデータ構造を変更せずに、Couchbase Serverへそのまま移行した場合、論理的に同じクエリを用いることが可能です。

念のため付け加えると、コレクションによって、リレーショナルデータベースにおけるテーブルのように、ドキュメントの構造が一律に規定される(テーブルスキーマが課せられる)わけではありません。
ドキュメント指向データベースとしての柔軟性は損なわれることはありません。

=== スコープの存在意義

スコープを用いてコレクションをグループ化することによって、エンティティーのグループを管理することができます。

スコープは、リレーショナルデータベースにおけるスキーマ相当の位置を占めることになります。

Couchbase Serverは、スコープとコレクションによってアクセス権限を管理する機能を提供しています。これによって、マルチテナンシーが容易になります。

//table[EN64bPC1sa][リレーショナルデータベースとCouchbase Serverの対比]{
リレーショナルデータベース	Couchbase Server
----------------------------
サーバー	クラスター
データベース	バケット
スキーマ	スコープ
テーブル	コレクション
レコード(ロウ)	ドキュメント
//}


====[column]スコープとコレクションの用語法について

 「コレクション」という表現は、プログラミングの分野でデータ構造を表す用語として広く浸透しているため、データベースに用いられていることに違和感を持つ人もいるかもしれません。
 ご存知ない方へ向けてお伝えすると、同じドキュメント指向データベースであるMongoDBでも、ドキュメントの格納単位として「コレクション」という用語が用いられています。
 Couchbase ServevrはMongoDBに対して、その登場自体もドキュメントをグループ化するための機能の追加も後発ですが、いたずらに異なる用語を導入するよりも好ましいといえるかもしれません。

「スコープ」という表現は、SQLにおけるコンテキストとの関係で理解することができます。SQLクエリは、特定のスキーマのコンテキストの元、実行されます。
このとき、テーブル名が一意に定まるのは、そのコンテキストの「スコープ」においてです(「スコープ」と「コンテキスト」という用語はプログラミング言語でも、テーブル名のかわりに変数に関して、同じ用法で用いられます)。
ちなみに、MongoDBにはスコープに該当する機能が存在しないため、用語の比較自体が成り立ちません。

====[/column]

=== 機能

スコープとコレクションによるデータ管理によって、以下が可能になります。

//blankline

 * スコープやコレクションごとにアクセス権を割り当てる。
 * コレクションにインデックスを付ける（スコープにインデックスを付けることはできません）。

//blankline

また、Couchbase Serverの以下の機能において、スコープとコレクションを利用して、操作の対象を限定することが可能です。

//blankline

 * N1QLクエリ
 * XDCR
 * バックアップ
 * 統計情報（cbstatsコマンド）


=== 基本仕様

 * クラスターごとに最大1000個のスコープを作成できます。
 * クラスターごとに最大1000個のコレクションを作成できます。

 * コレクション名は、スコープ内で一意である必要があります。
 * アイテム名(ドキュメントキー)は、コレクション内で一意である必要があります。


=== 作成・管理

スコープとコレクションは、以下を使用して作成・管理できます。

//blankline

 * Webコンソール
 * CLI
 * REST API
 * N1QL
 * SDK


=== デフォルトのスコープとコレクション

すべてのバケットには、デフォルトのスコープが自動的に与えられ、そのスコープにはデフォルトのコレクションがあります。それぞれ、@<tt>{_default}という名前を持っています。

特定のスコープおよびコレクションを指定しない場合は、@<tt>{_default}スコープの@<tt>{_default}コレクションが指定されているものとされます。

また、以下に注意する必要があります。

//blankline

 * デフォルトのスコープは削除できません。
 * デフォルトのコレクションは、CLIまたはREST APIのいずれかを使用して削除できます。 ただし、一旦削除すると、再作成できなくなります。

//blankline

スコープとコレクションが初めて導入された7.0より前のバージョンのCouchbase Serverから7.0へアップグレードすると、既存のすべてのドキュメントは、@<tt>{_default}スコープの@<tt>{_default}コレクションに自動的に配置されます。
これにより、アップグレード時のバージョン間の整合性が維持されます。

=== ユーザー定義のスコープとコレクション

ユーザー定義のスコープとコレクションに名前を付ける際は、以下の条件を満たす必要があります。

//blankline

 * 長さは1〜30文字。
 * 大文字と小文字が区別されます。
 * アルファベット(A-Z、a-z)、と数字(0-9)、記号として、@<tt>{_}, @<tt>{-}と@<tt>{%}のみを含めることができます（ただし、@<tt>{_}または@<tt>{%}で始めることはできません）。

//blankline

スコープ内の名前空間は、他のスコープの名前空間から独立しています。同一バケット内の複数のスコープに同じコレクション名が存在することができます。
同様に、コレクション内の名前空間は、他のコレクションの名前空間から独立しています。その結果、あるバケット内の同じ(または、異なる)スコープの複数のコレクションに、同じドキュメントキーが存在することができます。



//footnote[a55c3a0e13fc128db7ae95f8a1953f19][https://docs.couchbase.com/server/current/learn/data/scopes-and-collections.html]

== 有効期限設定

ドキュメントには有効期限を設定することができます。

====[column]エディションによる差異

有効期限設定は、エンタープライズエディションでのみ利用可能です。

====[/column]

Time To Live(TTL)をドキュメントに指定することによって、指定された期間の完了後に、そのドキュメントは利用できなくなります(削除されます)。
有効期限設定が有効化されている場合、ドキュメントは、TTLで指定された秒数の間存続します。

TTLは、CouchbaseバケットとEphemeralバケットで利用可能です。

=== バケットおよびコレクションレベルの設定

ドキュメントの有効期限は個々のドキュメント単位で設定するだけではなく、バケットおよびコレクションレベルで設定することが可能です。

バケットおよびコレクションレベルの設定は、バケット/コレクション内のドキュメントの最大有効期限を決定します。異なるレベルで有効期限が複数設定されている場合、より短い有効期限が採用されます。

バケットおよびコレクションレベルの設定は、設定変更以降に追加されたドキュメントに対して適用されます。

=== デフォルト設定

デフォルトでは、有効期限設定は有効化されません。デフォルトのTTLの値は0であり、これはTTLが無効であることを示します。
