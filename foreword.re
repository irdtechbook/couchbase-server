

= はじめに

== Couchbaseについて

Couchbaseという名称で名指される技術には、大きくいって、次の二つがあります。

 * Couchbase Server
 * Couchbase Mobile

Couchbase Serverは、分散JSONドキュメントデータベース（distributed JSON Document database）@<fn>{7b7c6748332542cc032aa96a4fa60999}です。Couchbase Mobileは、Couchbase LiteとSync Gatewayという二つの技術を包含する呼称です。Couchbase　Liteは、モバイル端末および組み込みデバイス用のネイティブドキュメントデータベース(native document database for mobile and embedded devices)@<fn>{7b7c6748332542cc032aa96a4fa60999}です。Sync Gatewayは、Couchbase MobileのデータをCouchbase Serverと同期するために使うことができるコンポーネント（components to sync to Couchbase Server）@<fn>{7b7c6748332542cc032aa96a4fa60999}です。

====[column]
本書では、題名にある通り、Couchbase Serverを扱います。Couchbase Liteは、組み込み用ネイティブデータベースとして、プラットフォーム毎に開発されています。その意味でも明らかな通り、Couchbase ServerとCouchbase Liteとは、それぞれ明らかに異なるプロダクト/テクノロジーであり、「ドキュメントデータベース」であることは共通しているものの、Couchbase Serverに関する本書の記述を、そのままCouchbase Liteへ援用することはできません。 

====[/column]

=== Couchbase Serverについて

Couchbase Serverは、ドキュメント指向データベースにカテゴライズされるNoSQLデータベースであり、シェアードナッシング型の分散アーキテクチャーを持ちます。

NoSQLと呼ばれるデータベースは、ビッグデータの集計処理や、IoTのようなリアルタイムかつ大量のデータ投入等、データベース毎にカバーするユースケースが異なっていますが、Couchbase Serverはデータに対して、リード・ライトの両方のアクセスを、低遅延で実現する必要のある（つまり、インタラクティブな）アプリケーションのバックエンドとして用いられるために設計（最適化）されています。

=== エディションとオープンソースプロジェクト

Couchbase Serverは、エンタープライズエディションとコミュニティエディションの、二つの形態でバイナリが提供されており、その基盤としてオープンソースプロジェクト@<fn>{https://github.com/couchbase}が存在しています。

コミュニティエディションは、一部の特別な機能を除いて、エンタープライズエディションの主要な機能を利用することができ、それらの機能について移植性が確保されています。つまり、コミュニティエディションを利用しているアプリケーションは、コードを変更することなく、エンタープライズエディションに移行可能です。一方で、エンタープライズエディションには、エンタープライズレベルの高い性能要件を満たすために、内部機構においてコミュニティエディションとはいくつかの違いが存在しています。
また、コミュニティエディションでは、クラスターを構成するノード数についても制限（最大5ノードまで）があります。

コミュニティエディションは、エンタープライズエディションのメジャーリリースと同時にリリースされますが、マイナーバージョンは、数か月遅れてリリースされ、メンテナンスリリースについても、限定されています。

====[column]
本書の記述は、基本的にコミュニティエディションに基づいていますが、エンタープライズエディションに固有の機能についてもコミュニティエディションとの差分を示す目的にて紹介しています。

====[/column]

== 関連データベース技術

Couchbase Serverを理解するための前提知識として、以下の用語について把握しておくと良いでしょう。

=== ドキュメントデータベース

ドキュメント指向データベース、ドキュメントストアとも呼ばれます。

==== 参考情報
Wikipedia Document-oriented database@<fn>{30f72b9ab8136d3edc4c76b432c8d9ea}

=== キーバリューストア

キーバリューデータベースとも呼ばれます。また、KVSという略称も広く用いられています。

==== 参考情報
Wikipedia Key-value database@<fn>{b435488410f1809f8b131aed3a6fe2bb}

=== インメモリデータベース

メモリ内でデータを管理することによって、高速な応答性能を実現するデータベース。一般に、データベースが、データ永続化装置（ハードディスク等）を用いた永続性と一貫性を保証しているのに対して、インメモリデータベースを利用する際には、永続化の要件は必要に応じ外部で実現される。あらかじめ永続化されているデータの純粋なキャッシュとして用いるのではなく、動的に更新されるデータベースとしてインメモリデータベースを用いる際には、メモリ上のキューを介して外部でデータを永続化することによって、事後的な一貫性を実現しながら、永続化装置の性能に影響されずに、インメモリデータベース本来の応答性能を担保するアーキテクチャーが用いられるのが一般的。

==== 参考情報
Wikipedia In-memory database@<fn>{f84c5e736c6060f55bd27cf70f1ea253}

=== リレーショナルデータベース

日本語では、関係データベース、と呼ばれます。関係モデル（リレーショナルデータモデル）に基づいて、データを扱います。

==== 参考情報
Wikipedia Relational database@<fn>{86b6b5174c671988f4e5dba65702a9c0}

=== クエリ

データベースにおけるクエリは、データベースへの問合せ（Query）として、普通名詞化されています。関係データベースに対するクエリの構文を定めた言語としてSQL(Standard Query Language)があります。クエリ言語を用いたデータベースへのクエリは、情報検索（データの抽出・集計）のみではなく、追加、削除、更新を含んでいます。

==== 参考情報
Wikipedia Query@<fn>{5dbbc36d7860bdcf33f45d112820924c}

=== トランザクション

データベースにおけるトランザクションは、原子性、一貫性、分離性、永続性を備えたデータへの一連の操作をいいます。

==== 参考情報
Wikipedia Database transaction@<fn>{91753e6ff342353eeab59f5277c76518}

 
//footnote[https://github.com/couchbase][https://github.com/couchbase]

//footnote[7b7c6748332542cc032aa96a4fa60999][https://developer.couchbase.com/open-source-projects/]

//footnote[30f72b9ab8136d3edc4c76b432c8d9ea][https://en.wikipedia.org/wiki/Document-oriented_database]

//footnote[b435488410f1809f8b131aed3a6fe2bb][https://en.wikipedia.org/wiki/Key%E2%80%93value_database]

//footnote[f84c5e736c6060f55bd27cf70f1ea253][https://en.wikipedia.org/wiki/In-memory_database]

//footnote[86b6b5174c671988f4e5dba65702a9c0][https://en.wikipedia.org/wiki/Relational_database]

//footnote[5dbbc36d7860bdcf33f45d112820924c][https://en.wikipedia.org/wiki/Query]

//footnote[91753e6ff342353eeab59f5277c76518][https://en.wikipedia.org/wiki/Database_transaction]

//footnote[6d51085d2247018dc350ae16c5d1919e][https://developer.couchbase.com/open-source-projects/]
