

=[notoc] はじめに

== Couchbaseについて

Couchbaseという名称で名指される技術には、大きくいって、次のふたつがあります。

 * Couchbase Server
 * Couchbase Mobile

Couchbase Serverは、分散JSONドキュメントデータベース（distributed JSON Document database）です(ここでの英語による引用は、Open Source Projects from Couchbase@<fn>{7b7c6748332542cc032aa96a4fa60999}より)。Couchbase Mobileは、Couchbase LiteとSync Gatewayというふたつの技術を包含する呼称です。Couchbase Liteは、モバイル端末および組み込みデバイス用のネイティブドキュメントデータベース(native document database for mobile and embedded devices)です。Sync Gatewayは、Couchbase MobileのデータをCouchbase Serverと同期するために使うことができるコンポーネント（components to sync to Couchbase Server）です。

====[column]Couchbase ServerとCouchbase Lite

Couchbase Liteは、組み込み用ネイティブデータベースとして、プラットフォーム毎に開発されています。
そこから明らかな通り、Couchbase ServerとCouchbase Liteはそれぞれ異なる技術を基盤としています。「ドキュメント指向データベース」であることは共通しているものの、Couchbase Serverに関する本書の記述を、Couchbase Liteへ援用することはできません。 

====[/column]

=== Couchbase Serverについて

Couchbase Serverは、ドキュメント指向データベースにカテゴライズされるNoSQLデータベースであり、シェアードナッシング型の分散アーキテクチャーを持ちます。

NoSQLと呼ばれるデータベースは、ビッグデータの集計処理や、IoTのようなリアルタイムかつ大量のデータ投入等、データベース毎にカバーするユースケースが異なっています。Couchbase Serverはデータに対して、リード・ライトの両方のアクセスを、低遅延で実現する必要のある（つまり、インタラクティブな）アプリケーションのバックエンドとして用いられるために設計（最適化）されています。

====[column]本書で扱うCouchbase Serverのバージョン

本書の記述は、2021年７月にリリースされた、Couchbase Server 7.0@<fn>{release-700}に基づきます。
なお、本書執筆時点の最新バージョンは、2021年９月にリリースされた、バグフィックスを含むメンテナンスリリースである、Couchbase Server 7.0.1@<fn>{release-701}です。

====[/column]

//footnote[release-700][https://docs.couchbase.com/server/current/release-notes/relnotes.html#release-700]

//footnote[release-701][https://docs.couchbase.com/server/current/release-notes/relnotes.html#release-701]

=== エディションとオープンソースプロジェクト

Couchbase Serverは、エンタープライズエディションとコミュニティーエディションの、ふたつの形態でバイナリが提供されており、その基盤としてオープンソースプロジェクト@<fn>{https://github.com/couchbase}が存在しています。

コミュニティーエディションは、一部の特別な機能を除いて、エンタープライズエディションの主要な機能を利用することができ、それらの機能について移植性が確保されています。つまり、コミュニティーエディションを利用しているアプリケーションは、コードを変更することなく、エンタープライズエディションに移行可能です。一方でエンタープライズエディションには、エンタープライズレベルの高い性能要件を満たすために、内部機構において、コミュニティーエディションとの違いがいくつか存在しています。
また、コミュニティーエディションでは、クラスターを構成するノード数についても制限（最大5ノードまで）があります。

コミュニティーエディションは、エンタープライズエディションのメジャーリリースと同時にリリースされます。マイナーバージョンは数か月遅れてリリースされ、メンテナンスリリースについても、限定されています。

====[column]エディションに関する本書の記載方針

本書の記述は、基本的にコミュニティーエディションに基づいていますが、エンタープライズエディション固有の機能についても、エディションによる差異を示しながら紹介しています。

====[/column]


== 関連データベース技術

本書の記述は、データベース一般、特にリレーショナルデータベースについての知識を前提としています。

また、Couchbase Serverとの比較対照のために他のNoSQLデータベースについて触れる場合、個々のデータベースについて精通していることは想定していませんが、NoSQL全般に関する一定の知識を前提としている部分があります。

ここでは、このような関連するデータベース技術について素描を試みつつ、詳細情報への参照先を掲載します。

=== ドキュメント指向データベース

ドキュメント指向データベース(Document-oriented database@<fn>{30f72b9ab8136d3edc4c76b432c8d9ea})は、ドキュメントストアとも呼ばれます。
ドキュメント指向データベースというカテゴリーには、XMLなどの様々なフォーマットを扱うデータベースが含まれますが、Couchbase Serverは、JSON形式のデータを扱うドキュメント指向データベースです。


=== キーバリューデータベース

キーバリューデータベース(Key-value database@<fn>{b435488410f1809f8b131aed3a6fe2bb})は、キーバリューストアとも呼ばれ、KVSという略称も広く用いられています。
連想配列、ディクショナリ、あるいはハッシュテーブルとも呼ばれるデータ構造で、データを管理するデータベースです。


=== インメモリーデータベース

インメモリーデータベース(In-memory database@<fn>{f84c5e736c6060f55bd27cf70f1ea253})は、メモリー内でデータを管理することによって、高速な応答性能を実現するデータベースです。
一般に、データベースがデータ永続化装置(ハードディスク等)を用いた永続性を保証しているのに対して、インメモリーデータベースを利用する際には、永続化の要件は必要に応じ外部で実現されます。

=== リレーショナルデータベース

リレーショナルデータベース(Relational database@<fn>{86b6b5174c671988f4e5dba65702a9c0})は関係データベースとも呼ばれ、関係モデル（リレーショナルデータモデル）に基づいてデータを扱います。

リレーショナルデータベースでは、データは(複数の)表として管理され、この表は「テーブル」と呼ばれます。
一般に、表は行(カラム)と列(ロウ)からなりますが、テーブルの定義により決定される、単位データ(レコード)を構成する要素(フィールド)は「カラム」と呼ばれ、レコードは「ロウ」と呼ばれます。


=== クエリとインデックス

データベースにおけるクエリ(Query@<fn>{5dbbc36d7860bdcf33f45d112820924c})は、データベース管理システムへの問い合わせ一般の表現として普通名詞化されています。
リレーショナルデータベースに対するクエリの構文を定めた言語としてSQL@<fn>{wikipedia-sql}があります。
クエリ言語を用いたデータベースへのクエリは、情報検索(データの抽出・集計)のみではなく、データの追加、削除、更新処理を含んでいます。

データベースのインデックス(Index@<fn>{wikipedia-index})は、検索処理を高速化するためのデータ構造であり、インデックス情報を保管するためのストレージスペースと、データ書き込み時のインデックス情報更新のためのリソース消費を伴います。

=== トランザクション

データベースにおけるトランザクション(Database transaction@<fn>{91753e6ff342353eeab59f5277c76518})は、原子性、一貫性、分離性、永続性を備えたデータへの一連の操作をいいます。

 
//footnote[https://github.com/couchbase][https://github.com/couchbase]

//footnote[7b7c6748332542cc032aa96a4fa60999][https://developer.couchbase.com/open-source-projects/]

//footnote[30f72b9ab8136d3edc4c76b432c8d9ea][https://en.wikipedia.org/wiki/Document-oriented_database]

//footnote[b435488410f1809f8b131aed3a6fe2bb][https://en.wikipedia.org/wiki/Key%E2%80%93value_database]

//footnote[f84c5e736c6060f55bd27cf70f1ea253][https://en.wikipedia.org/wiki/In-memory_database]

//footnote[86b6b5174c671988f4e5dba65702a9c0][https://en.wikipedia.org/wiki/Relational_database]

//footnote[5dbbc36d7860bdcf33f45d112820924c][https://en.wikipedia.org/wiki/Query_language]

//footnote[wikipedia-sql][https://en.wikipedia.org/wiki/SQL]

//footnote[wikipedia-index][https://en.wikipedia.org/wiki/Database_index]

//footnote[91753e6ff342353eeab59f5277c76518][https://en.wikipedia.org/wiki/Database_transaction]

//footnote[6d51085d2247018dc350ae16c5d1919e][https://developer.couchbase.com/open-source-projects/]
