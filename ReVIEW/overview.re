= Couchbase Serverとは何か

== Couchbase Serverの歴史

Couchbase Serverの歴史の直接的な始まりは、Membase社とCouchOne社の合併により、Couchbase, Inc.@<fn>{about}が設立された2011年に遡ります。

Couchbase, Inc.は、Membase社とCouchOne社の合併により設立されました。
Membase社は、memcached@<fn>{memcached}プロジェクトのリーダー達により開発されたキーバリューストアMembaseを提供していました。CouchOne社は、ドキュメント指向データベースApache CouchDB@<fn>{couchdb}プロジェクトの主要メンバーを擁し、CouchDBの商用サポートを提供していました。


//footnote[memcached][https://memcached.org/]

//footnote[couchdb][https://couchdb.apache.org/]

//footnote[about][https://www.couchbase.com/about]

=== インメモリーKVSからJSONデータベースへ

2012年にリリースされたCouchbase Server 2.0により、Membaseを前身とするCouchabse Serverは、純粋なキーバリューストア(KVS)から、JSONデータベース(ドキュメント指向データベース)へと発展を遂げました。

Couchbase Serverは、JSONデータを扱うために必要な機能を提供する一方、キーバリューストアとしての側面を残しており、JSON以外のデータをバリューとして格納することも可能です。また、memcachedのように、揮発性のキャッシュとして利用するためのオプションも提供しています(デフォルトではデータをディスクに永続化します)。永続化機構において、メモリーファーストアークテクチャーが採用されており、読み込みおよび書き込みのデータ操作において、ミリ秒以下の応答性能を実現しています。

上に説明したCouchbase Serverのコア機能は、Dataサービスと呼ばれます。なお、Couchbase Serverでは、リレーショナルデータベースや、他の多くのデータベースでは、「データベース」と呼ばれるデータの格納先を「バケット」と呼びます。


====[column]本書におけるサービス名表記方針
Couchbase Serverは、Dataサービスをはじめとする複数のサービスから構成されています。本書では、一般名詞と明確に区別するため、「Dataサービス」のようにサービスの名称には英語表記を用いています。

====[/column]

=== クエリ

Couchbase Server 4.0にて、N1QL(Non 1st normal form Query Language)が導入され、Couchbase Serverに格納されているデータに対して、SQL準拠のクエリ言語を用いてアクセスすることができるようになりました。N1QLは、非第一正規形クエリ言語という意味を持ち、ニッケルと発音します。

N1QLクエリは、QueryサービスとIndexサービスで実現されます。

=== 全文検索

Couchbase Server 5.0にて、全文検索機能(Searchサービス)が導入されました。Searchサービスは、テキストデータに対する単語レベルのインデックス化による全文検索機能に加え、検索語との関連性に応じて文書をスコアリングし検索結果をソートしたり、検索結果中の該当箇所をハイライトするために必要なデータを合わせて提供するといった、アプリケーションに包括的な検索機能を実装するための技術を提供します。

また、Couchbase Server 6.6では、全文検索機能をN1QLクエリから直接利用することができるFlexインデックスが導入されています。

====[column]本書におけるSearchサービスの扱い
本書では、Searchサービスについては扱いません。

これは、全文検索は、Couchbase Server利用において、必ずしも中心的なユースケースとは言えず付加的なものであるのに対して、実践的に使うためには、ドキュメント指向データベースとはまた別の種類のシステムを扱うのに相当する知識が必要とされ、本書の持つ入門書的な性格とはそぐわないためです。

====[/column]

=== イベント処理
Couchbase Server 5.5にて、イベント処理機能(Eventingサービス)が導入されました。ユーザーは、「イベント-コンディション-アクション」モデルに基づいてトリガーされるJavaScriptを用いたユーザー定義関数を利用できるようになりました。ユーザーは、Couchbase Server内のデータ更新・削除(イベント)に対して、条件(コンディション)と、それに対するユーザー定義関数(アクション)を定義します。ユーザー定義関数では、Couchbase Serverのデータに対する操作のほか、REST APIコールによる外部サービス呼び出しを実行することが可能です。

====[column]エディションによる差異
Eventingサービスは、エンタープライズエディションでのみ利用可能です。

====[/column]

=== アナリティクス

Couchbase Server 6.0にて、Analyticsサービスが導入されました。この新しいサービスの導入により、Couchbase Serverは、OLTP(Online Transaction Processing)とOLAP(Online Analytical Processing)をシングルプラットフォームで実現するHTAP(Hybrid Transaction/Analytical Processing)環境として利用可能になりました。


Analyticsサービスにより、Couchbase Serverをバックエンドデータベースとして利用しているビジネスアプリケーションの性能に影響を与えることなく、ニアリアルタイムのデータを用いたアドホックな分析を行うことが可能になります。そのために、OLTP環境とOLAP環境を統合するETL処理を実装する必要はなく、また、分析用にデータのスキーマを変更する必要もありません。

Analyticsサービスは、アドホックな分析要件に適した形でデータを保持します。処理実装には、MPP(Massively Parallel Processing)技術の最新の成果が用いられており、大量のヒストリカルデータの集計が必要とされるような分析要件に対して、高い応答性能を実現しています。

Analyticsサービスが扱うデータは、従来クラスター内部のデータのみでしたが、Couchbase Server 6.6からは、別のCouchbase Serverクラスターのデータ、およびS3ベースのデータストレージにまで拡張されています。

Analyticsサービスは、分析エンジン機能のみを担います。UIの実現のために、外部のBIツールからAnalyticsサービスへアクセスするためのODBC/JDBCドライバーが、CDATA@<fn>{1433ffe25ca932a2c5c0242cfc86b3e3}社から提供されています。

====[column]エディションによる差異
Analyticsサービスは、エンタープライズエディションでのみ利用可能です。

====[/column]

=== スコープとコレクション

Couchbase Server 7.0において、バケット内のキースペースとして、スコープとコレクションが導入されました。
コレクションが導入される前は、Couchbase Serverには、RDBにおけるテーブルスキーマに相当するものがなかったため、データの種類を表現するためには、ドキュメントの内部にデータの種類に関する情報を持つ必要がありました。

コレクションは、RDBにおけるテーブル定義のように、データの構造に関して制約するものではないため、スキーマレスであることの柔軟性は失われません。

スコープとコレクションの導入により、RDBで管理されているデータをCouchbase Serverに移行することが、これまでと比べて、より簡単に行えるようになりました。



=== 運用やセキュリティ機能の拡充

また、Couchbase Server 7.0では、Backupサービスが導入されました。それまでのCouchbase Serverでは、バックアップ機能はコマンドラインツールとして提供されていましたが、Backupサービスの導入により、スケジューリングや、Webコンソールからの設定が可能になりました。

ここまで紹介した、新しいユースケースを実現する機能追加だけでなく、運用やセキュリティ等の面においても、Couchbase Serverは、その登場以来、進歩を続けています。

====[column]エディションによる差異
Backupサービスは、エンタープライズエディションでのみ利用可能です。

====[/column]


== NoSQLと現在の技術動向

NoSQLというキーワードが登場してから、長い年月が経ちました。ここでは、NoSQLという技術の現在について整理します。

まずは、現在の情報技術を取り巻く環境について、振り返ることから始めます。

=== DX (Digital Transformation)

デジタルトランスフォーメーション〜DXについて、経済産業省資料「『DX推進指標』とそのガイダンス」@<fn>{meti_go_jp_dx}では、以下のように定義されています。

//quote{

企業がビジネス環境の激しい変化に対応しデータとデジタル技術を活用して、顧客や社会のニーズを基に製品やサービス、ビジネスモデルを変革するとともに業務そのものや組織、プロセス、企業文化・風土を変革し競争上の優位性を確立すること

//}

ここで、トランスフォーメーション、つまり変革の対象とされているものは２つあります。１つは、企業が生み出し、顧客に提供する製品やサービス、そしてもう１つは、企業内部の業務プロセスです。
そして、この変革は、「顧客や社会のニーズを基に」したものであるとされています。

極めて、当たり前のことが語られているようにみえます。一方で、こう考えることもできるのではないでしょうか、つまり、これまでも確かに情報技術によって変革が行われてきた、ただしそれは(十分に)「顧客や社会のニーズ」に基づくものではなかった、と。

=== SoE (System of Engagement)

SoE、エンゲージメント（のため）のシステム。つまりユーザーとの「つながり」を掲げたシステム。この「SoE」が語られる文脈では、旧来からあるシステムは、SoR（システムオブレコード）、「記録」（のため）のシステム、と呼ばれることになります。

ここで重要なのは、SoRとSoEとの線引きは、「更新系」と「参照系」という古のシステム区分とは異なるということです。「更新系」と「参照系」との関係では、「参照系」システムは、いわば「更新」以外の領域として見出されており、それらはシステムの「機能」から見た区分であるといえます。SoEとSoRとの差異は、システムの「目的」、さらにいえば、システムがどのような背景から発想されているか、にあるといえます。

新しい言葉の誕生に立ち会う時、私たちは、その概念が登場する前の風景がどういったものだったか、に思いを巡らすことになります。これまでの（私たちにとって当たり前と思われていた）システムは、（その時点の技術でできること、つまり）システムにとって得意なことから発想されており、ユーザー（とのエンゲージメント）から発想されたものでなかった、そう考えることも出来そうです。

=== NoSQLの登場まで

NoSQL登場以前には、データベースの分類といった時、RDBとDWH（データウェアハウス）という区分が、最も定番といえるものでした。そしてそれは、基盤系と情報系というシステムの分類に対応していました。また、OLTP（OnLine Transaction Processing）とOLAP（OnLine Analytical
Processing）という用語を、ここで思い起こしても良いでしょう。

NoSQL（NOまたは、Not Only SQL）という言葉の登場により、データベースの分類に、混乱が招き入れられました。実際「従来のものと異なる」という表現は、カテゴリーとして意味のあるものではなく、純粋なバズワードとして機能することしかできないのは当然でした。乱暴に言えば、NoSQLは、「ビッグデータ」との組み合わせによる類推から（蓄えられたヒストリカルデータの利用という観点から）、広い意味で「情報系」というカテゴリーに投げ入れられることになったとさえいえます。その意味で、結局「Nice To Have」であり、いたずらにDWHよりも複雑なもの、という印象に終始したとさえいえるでしょう（ここでの表現は、極端な断言に聞こえることは承知していますが、ある特定の文脈の提示を意図してのものです）。

=== SoEとNoSQL

ここで、DXの隆盛を鑑みつつ、SoEのひそみにならって、エンゲージメントのためのデータベース、について考えてみたいと思います。

エンゲージメントのためのシステムにふさわしいデータプラットフォームとはどのようなものでしょうか？SoEとSoRの境界を考えてみる時、例えば、ネットショッピングにおけるユーザーの行動という視点から次のようにイメージすることができます。

 * SoRの範囲: 商品購買・決済

 * SoEの範囲: 商品の検索、推薦、カート保管、レビューなど

上記のイメージを援用すれば、ネットショッピングという限定を外して考えても、多くのアプリケーションがSoEの範囲に含まれることがわかるのではないかと思います。

ここで、SoEとSoRの間のシステムの性格の違いを、以下の３点に整理してみたいと思います。

 * @<strong>{ユーザーの規模} システムを利用するユーザーの規模は、SoRの非ではない。さらにシステムは、増え続けるユーザー数・利用量に対応していく必要がある。
 * @<strong>{サービスの柔軟性} 新しいサービスを素早くリリースすることが重要であり、また、サービスは常に環境、つまりユーザーとの関係の変化に応じて更新されなければならない。
 * @<strong>{要求される応答速度} サービスの多様化に合わせて、システムは複雑になり続ける。それに応じて、システムの各構成要素、例えば、データプラットフォームの応答速度として許容される時間は短くなる。

そして、これらは「NoSQL」と呼ばれるデータベースの以下の技術要素にそれぞれ対応している、と言えます。

 * 分散アーキテクチャー
 * スキーマレス
 * インメモリー

以下、それぞれの技術要素について、順に見ていきます。

//blankline

 @<strong>{分散アーキテクチャー}
 
 多くのNoSQL(およびNewSQL)データベースが共通して分散アーキテクチャーという特徴をもっています。それらのデータベースは、従来のモノリシックなアーキテクチャーからなるデータベースとは異なり、スケールアップによるリソース増強のみではなく、スケールアウトモデルによって、必要に応じた、サービスを停止する必要のない、柔軟なリソースの拡張(あるいは縮小)が可能です。

//image[scale-out][スケールアップモデルとスケールアウトモデル]{
//}

 (画像は、 Moving from Relational to NoSQL: How to Get Started@<fn>{relational-no-sql-wp}より引用)

//blankline
 
 @<strong>{スキーマレス}
 
スキーマレスという特徴も、多くのNoSQLに共通しています。任意のキーを用いてデータを格納するキーバリューストア（例えば、Redis）と、列指向データベース（例えばHBase、Cassandra）、JSONというツリー構造のデータ記述フォーマットを用いるドキュメント指向データベース(例えば、MongoDB)は、全てNoSQLに分類されますが、スキーマレスという共通の特徴を持ちます。データ構造の柔軟性についていえば、ドキュメント指向データベースには、ツリー構造を表現でき、アプリケーションのデータ（ビジネスドメインオブジェクト）をそのまま格納することができる、という他のデータベースにはない利点があります。

//image[schemaless][リレーショナルデータベースのテーブルデータ構造とドキュメント指向データベースのJSONデータフォーマット]{
//}

 (画像は、 Moving from Relational to NoSQL: How to Get Started@<fn>{relational-no-sql-wp}より引用)

//footnote[relational-no-sql-wp][https://resources.couchbase.com/c/relational-no-sql-wp?x=Y7B0ca]

//blankline
 
 @<strong>{インメモリー}
 
インメモリーについては、オープンソースのNoSQLとしては、Redisが代表的と言えます。一般には、インメモリーデータベースは、（単体でアプリケーションのキャッシュとして用いるのでなければ）永続化層を担う他のデータベースと組み合わせて利用することによって、高い応答性能という特性を持つデータプラットフォームを実現することができます。

//image[cache][キャッシュとデータベースの併用によるデータアクセスレイヤー実現例]{
//}


 (画像は、 High Performance with Distributed Caching@<fn>{high-performance-wp}より引用)

//footnote[high-performance-wp][https://resources.couchbase.com/c/high-performance-wp?x=s9hNYZ##]

//blankline


=== SoEのためのデータベースとしてのCouchbase Server

Couchbase Serverは、上で述べてきた３つの技術的要素を全て兼ね備えており、エンゲージメントのためのデータベースとして相応しい特徴を持っている、といえます。

NoSQLにカテゴライズされるデータベースには、それぞれ、ビッグデータの集計処理や、リアルタイムかつ大量データの投入等のように、異なった典型的なユースケースがあります。
Couchbase Serverは、データに対して、リード/ライトの両方のアクセスを、低遅延で実現する必要のある（つまり、インタラクティブな）アプリケーションのバックエンドとして用いられるために設計（最適化）されており、エンゲージメントのための(システムのバックエンド)データベースとして相応しいといえます。

== メモリーファーストアーキテクチャー

=== インメモリーDBと永続化

ここでは、Couchbase Serverのメモリーファーストアーキテクチャーについて理解するため、まずインメモリーDBという技術一般について検討します。
また、その具体的な実現としてOracl CoherenceとRedisを取り上げます。これらは、いずれもインメモリー分散KVS(キーバリューストア)というカテゴリーに入ります。また、どちらもコミュニティエディションの存在するOSSベースのソフトウェアです。

インメモリーDBは、その名の通りデータをメモリーに格納します。アプリケーション(データベース利用者)からみたインメモリーDBの性能面の特徴としては、永続化装置の性能がボトルネックにならないデータベースであると言えます(もっとも、インメモリーDBはそもそも永続化の機構を担っていないため、これ自体はさほど意味のない表現ですが)。このデータベースとしての特徴を、永続化を担う他のシステムと組み合わせることによって、従来のデータベースとは異なるレベルの高い応答性能を実現するアーキテクチャーを構築することができます。


=== データ永続化パターン

インメモリーDBを、単にアプリケーション層のキャッシュを実現するだけの「分散キャッシュ」と呼ばれるものと区別して考える場合、データの永続化という課題が浮かび上がります。

ここで、データ永続化には以下のような、よく知られたパターンがあります。

 * @<strong>{キャッシュアサイド} アプリケーションは、データへの要求を受けた際に、データがキャッシュの中にあるか調べ、なければデータベースからロードし、将来の別の要求のためにキャッシュの中に保存する。キャッシュアサイドのパターンでは、専用のキャッシュ層が設けられておらず、アプリケーション自身がキャッシュに関連する操作を行う。

 * @<strong>{リードスルー} キャッシュ層が、アプリケーションからデータの読み込み要求を受けた際に、そのデータがキャッシュの中にあるかどうかを調べ、なければデータベースからロードし、将来の別の要求のためにキャッシュの中に保存する。この構成により、キャッシュ層における最適化（例えばデータのプリフェッチ）も可能になる。

 * @<strong>{ライトスルー} キャッシュ層が、アプリケーションからデータへの書き込み要求を受けた際に、データベースへの書き込みと、キャッシュの更新を同時に行う。

 * @<strong>{ライトビハインド} キャッシュ層が、アプリケーションからデータへの書き込み要求を受けるのはライトスルーと同様だが、キャッシュ層にキュー/スレッドを備え、アプリケーションとキャッシュ層との関係（レスポンスタイム、スケーラビリティ）と、キャッシュ層とデータベースとの関係を分離する。この関係の分離により、同一エントリへの複数の更新がバッチとしてまとめられる、データベースの障害との直接の影響関係を取り除く、などの利点が生まれる。

=== バックアップとしての永続化

データ永続化の課題をキャッシュ層における障害発生時のデータ復旧のためのバックアップという観点で捉えることもでき、以下のようなアプローチが存在します。全体の論旨の上では傍系的な話題となりますが、永続化のテーマを網羅的に概観するため整理します。

 * @<strong>{スナップショット} 定点におけるデータをバックアップファイルとして保存する機能。

 * @<strong>{書き込みログ} RDBにおけるいわゆるWAL/REDOログ。Redisでは、AOF(Append Only File)と呼ばれる。

=== Oracle CoherenceとRedisの永続化手法

冒頭にあげた、２つのインメモリーDBについて、永続化手法との関係を整理します。

 * @<strong>{Coherence}@<fn>{dee7910fc6a7f7780ac7831f794eda19}では、上記のデータ永続化パターン（リード/ライトスルー、ライトビハインド）を想定した実装手法が確立されています。

 * @<strong>{Redis}@<fn>{522195171ed14f78e1f33f84a98f0de6}をキャッシュとして使い、ドキュメント指向データベースであるMongoDBと連携することによって、永続化を実現する手法が見られます。

=== 永続化機構を備えたインメモリーDBとしてのCouchbase Server

これまで見たように、インメモリーDBを利用する場合、通常データの永続化は、ユーザーの選択した方法によるインテグレーションにより実現されます。Couchbase Serverのユニークなところは、メモリー「ファースト」アーキテクチャーの採用にあります。
ユーザーは、メモリー上のデータへのアクセス（ディスク利用による性能制約からの開放）による応答性能を享受しながら、データは、あたかもごく一般的なデータベースのように、ディスク上に保存されている状態が、シングルプラットフォームで実現されています(ここで、「シングルプラットフォーム」という表現は、このようなキャッシュと永続化の両方を満たすための典型的なアーキテクチャーとして、キャッシュレイヤーと永続化レイヤーを２つの異なるテクノロジーによって実現するケースとの対照として用いられています)。

Couchbase Serverの内部的な実装は、いわば上述の「ライトビハインド」のパターンに相当します。さらに、一貫性(Durability)要件に応じ、永続化装置(および複製)に対して、同期的に更新するオプションを提供しています。オプションの選択はAPIコール単位であり、システム全体で、ライトスルーかライトビハインドかを固定する必要はありません。

以上、説明したアーキテクチャーのイメージを示します。

//image[memory_first_arch][Couchbase Serverのメモリーファーストアーキテクチャー]{
//}

(画像は、Couchbase Under the Hood: An Architectural Overview@<fn>{9a6cce3336b02e30ca1fb60d9b70e9ad}より引用)

====[column]純粋なキャッシュとしてのCouchbase Server
Couchbase Serverは、Memcachedプロジェクトのメンバーにより開発されたMembaseをその前身としており、Memcachedのユースケースへの適応が意識されています。
そうした背景から、メモリーのみを利用して永続化要件のないデータを扱うこともサポートされています。

本書では、Couchbase Serverのこの側面については、本来的なデータベースとは異なるユースケースとして、最小限の扱いに留めています。

====[/column]

== JSONデータベース

ここでは、JSONデータを扱うドキュメント指向データベースが、なぜ現在のアプリケーション開発において、重要なのかを見ていきます。

=== JSONの特徴

まず、先行する技術であるXMLとの比較から、JSONの特徴を整理します。

 * @<strong>{XML} ユーザーが定義したタグを用いて文章構造を記述するマークアップ言語。
 * @<strong>{JSON} キーと値(文字列、数値、真偽値、オブジェクト、およびそれらの配列)のペアの集まり（オブジェクト）によって、ユーザーが定義した構造を持つデータを記述するデータ記述言語。

JSONとXMLに共通してみられる特徴として、以下があります。

 * データ記述言語であり、データと同時に、データの意味と構造を記述することができる。
 * テキストベースのデータ交換用フォーマットである。
 * データ構造として、ツリー構造を持つ。ツリー構造は、ネットワーク構造以外のほとんどのデータ表現のニーズに対応できる(RDBで用いられているテーブル構造や、キーと値のペア等、より単純なデータ構造は、ツリー構造に包含される）。
 * プログラミング言語を問わず利用できる。
 * 仕様が固まっており、十分な歴史・実績を持つ。

さらに、XMLに対するJSONのメリットとして、以下があります。

 * XMLよりも軽量。
 * プログラミング言語との親和性が高い。JSONは、JavaScript Object Notationの略紀であることが示すように、構文はJavaScriptにおけるオブジェクトの表記法に由来しており、その他の多くのプログラミング言語が共通して持つデータ構造とも直接的な対応関係を持っている。

=== JSONとWebフロントエンド技術との親和性

XMLが、そして後にはJSONが、広く浸透した理由として、その「テキストベースのデータ交換用フォーマット」という特徴が、Web技術と親和性が高かったことが挙げられます。

「データ交換」のユースケースの中でも、特に重要だったのが、Webブラウザー上での扱いだったといえます。Webブラウザー上で実行可能なスクリプト言語の仕様として登場した、非同期更新技術(Ajax@<fn>{1a4091951f5fb1f307e2ee166df63c1d})は、Webアプリケーションに、ネイティブアプリケーションと遜色のない、ユーザー体験を提供することを実現しました。この非同期更新技術のデータ交換フォーマットとして用いられたのが、当初はXMLであり、後にはJSONが、実質的に取って代わることになります。

その結果が、現在のSPA(Single Page Application)@<fn>{4cbf4c2bb4d7602315f6b0d7af6745be}やPWA(Progressive Web Application)@<fn>{7421ae409426b8fcd3cb845fe195a411}の隆盛につながっています。
そして、このような新しいUI技術がなぜ重要かと言えば、アプリケーションのユーザビリティの向上によりページビューの増加や離脱率の減少に繋がるからに他なりません。@<fn>{bef41e78b4981370f88d07cbb1507da7}


=== Webアプリケーション開発と高い親和性を持つCouchbase Server

Webブラウザー上で実行可能なスクリプト言語として登場したJavaScriptは、サーバーサイドでも用いられるようになっています。

一方、JSONは、XMLにおいては広く浸透したとは言い難いデータ管理の目的においても、そのデータ記述言語としての（これまでのデータ管理の主流であったRDBに遥かに勝る）柔軟性により、多くのデータベースにおいて採用されることになりました。

JSONデータを扱うドキュメント指向データベースであるCouchbase Serverを用いることにより、Webフロントエンドとデータベースの両方で、更にはサーバーサイドを含めて、JavaScript/JSONを使ったアプリケーション開発が可能になります。


//footnote[bef41e78b4981370f88d07cbb1507da7][ディーエムソリューションズ株式会社コラム「PWA（Progressive Web Apps）とは？メリットと実装事例について」(https://digital-marketing.jp/seo/what-is-progressive-web-apps/)では、新しいUI技術（SPA, PWA）が、ページビューの増加や離脱率の減少へ与える影響について、具体的な数字を交えて紹介されています。]


== トランザクションとNewSQL

ここでは、データベース技術の重要なトピックであるトランザクションについて、データベース技術者が、NoSQLドキュメント指向データベースに触れる際に有益と思われる論点を整理しながら、歴史的な展望と併せて、見ていきます。

=== ドキュメント指向データベースにおけるトランザクションの原子性

ドキュメント指向データベースでは、トランザクションの原子性（Atomicity）は、ドキュメントという単位と一致しているということができます。

Couchbase Serverのドキュメント更新に対するアプローチとして、「get-and-lock API」で実現される悲観的（ペシミスティック）ロックと「check-and-set (CAS) API」による楽観的（オプティミスティック）ロックの２つがあります@<fn>{cf3d5fa42e3685b9552b8630edc07c55}。

//footnote[cf3d5fa42e3685b9552b8630edc07c55][ブログ記事「Optimistic vs Pessimistic Locking - Which One Should You Pick?」(https://blog.couchbase.com/optimistic-or-pessimistic-locking-which-one-should-you-pick/)にて、これらの違いと適用基準が解説されています。]


=== マルチドキュメントトランザクションの必要性

複数のデータ(エンティティ)の関係を、外部参照キーによって表現することは、ドキュメント指向データベースであっても、採用し得る設計です。
そのことを踏まえ、複数ドキュメントに跨がるトランザクションが必要なケースとして、次の２つについて考えてみます。

 * 複数の（関係のない）エンティティ間でのデータの受け渡し（例えば、口座間での送金）

 * 複数の（関係した）エンティティの同時更新（例えば、受注レコードと受注明細）

後者について言えば、ドキュメント指向データベースが扱う準構造化データでは、第一正規化が必須とされないため、必要なトランザクション境界を、１ドキュメントに含まれるデータの範囲と一致させる設計が可能です。

その一方で、NoSQLが従来想定されていた範囲を超えて有益であることが認められてくるに連れて、高度なトランザクションの実現に対する要望が高くなってきました。
その結果、NoSQLデータベースが持っていたトランザクションに関する制約を、データベース外部で解決するという試みも行われてきました。
Couchbase Serverについてのそのような例として、NEC@<fn>{wiki-nec}のInfoFrame Relational Store(IRS)@<fn>{66fe5150939f305857c0310ca092a9bc}があります。

//footnote[wiki-nec][https://en.wikipedia.org/wiki/NEC]

そうした時期を経て、昨今ではNoSQLデータベース自体に、マルチドキュメントトランザクションが実装されてきており、その背景には、NoSQLのユースケースの範囲が、大きく変わってきていることが示唆されています。

=== Couchbase Serverの分散トランザクション

Couchbase Serverのマルチドキュメントトランザクションは、複数ドキュメントに跨がるトランザクションを実現します。@<fn>{distributed-transactions-multi-node-and-multi-bucket}
Couchbase Serverのドキュメントは、物理的に複数のノードに跨がって管理されるので、当然ながら、トランザクションも複数ノードに跨がるものになります。
データ管理上の論理的側面における「マルチドキュメントACIDトランザクション(Multi-Document ACID Transactions@<fn>{b0d8e878210f45b315d30ab3106b7a0e})」は、分散アーキテクチャーの観点からの表現である「分散トランザクション」と並列的に成立しており、「分散マルチドキュメントACIDトランザクション(Distributed Multi-Document ACID Transactions@<fn>{d2d87c2fb49681980479824d0e447353})」という表現も用いられます。

以下に、トランザクションが、複数のノードにまたがって実行されるイメージを示します。

//image[transaction][Couchbase Serverの分散トランザクション]{
//}

(画像は、Couchbase Under the Hood: An Architectural Overview@<fn>{9a6cce3336b02e30ca1fb60d9b70e9ad}より引用)

//blankline

ここで注意しなければならないのは、Couchbase Serverのマルチドキュメントトランザクションは、楽観的平行性制御に基づくものであって、上で触れた単一ドキュメント更新の場合と異なり、悲観的ロックを選択することはできないということです。

=== NewSQL

NewSQLと呼ばれる新しいデータベースの登場は、現在におけるNoSQLの位置づけを見極める上でも、興味深いものだといえます。
その意味で、451 Group@<fn>{451_Group}により作成された以下の図を引用することは、意義深いと思われます。

//footnote[451_Group][https://en.wikipedia.org/wiki/451_Group]

図の中で、Couchbase Serverは「Document」と「KeyVakue」の両方のエリアに跨がる場所に位置づけられています。

//image[newsql][]{
//}

(画像は、NoSQL, NewSQL and Beyond@<fn>{90234a6d1bc62d7f4d443da72a796c21}より引用)

//blankline



NewSQLについての詳細な説明は、上記図の引用元や、その他の文献に譲りたいと思いますが、端的にいえば、Google Spanner@<fn>{google-spanner}とそのクローンを差すものと考えて良いかと思います（ここでは、その解釈とします）。
その特徴について要約すれば、分散アーキテクチャー（のメリット）を持ち（NoSQLから引継ぎ）ながら、あくまでRDBであること（つまり、トランザクションとSQLを備えていること）を打ち出しているように見えます。
例えば、Spannerについては、「Fully managed relational database with unlimited scale, strong consistency, and up to 99.999% availability.」@<fn>{f330266e643fbd2b7d2a10960b35a423}と謳われています。
クローンごとに打ち出し方が異なっているのが見てとれ、CouckroachDB@<fn>{d6032cdd340139ba8d35f5aec7d0d61e}では、「strong consistency」という技術的特徴を「mission-critical」という目的の形で表現している一方、TiDB@<fn>{d9883eb51d84bd0f4bfe3bafca1931b5}は、「distributed SQL database for elastic scale and real-time analytics」という表現を用いています。YugaByteDB@<fn>{e968a7936c71fff2010123dc01af46ab}は、「Single-Digit Millisecond Latency」を筆頭に挙げているのも興味深いところです。

トランザクションという点についていえば、Couchbase Serverのマルチドキュメントトランザクションが、リードコミッテッドの分離(Isolation@<fn>{Isolation_database_systems})レベルに基づいている一方、NewSQLのトランザクションは、より厳格(シリアライザブル)であることが大きく異なっています。これは、SQLが使えることと並び、NewSQLが、(新しい)SQL(/RDB)を標榜している背景にある、重要な特徴と言えるでしょう。
Couchbase Serverとの比較で言えば、SQL(拡張)クエリが使えるところは共通している一方で、明らかに異なる特徴が、このトランザクションの実装ということになります。


//footnote[Isolation_database_systems][https://en.wikipedia.org/wiki/Isolation_(database_systems)]

=== 楽観的ロックの何が問題か

楽観的ロックの何が問題か、以下の二点に集約して考えてみたいと思います。

 * トランザクション競合発生時の対応がリトライに基づくため、最終的に（集中時には）性能の問題に帰結する場合がある。
 * 悲観的ロックに基づくデータベースでの開発を常識としてきた開発者にとって、これまでの流儀を全く変えずに開発ができるわけではない。

高いトランザクションレベル（悲観的ロック）を取ることが絶対に重要で、システムに必要な要件であるユースケースがある一方（その場合、メリット・デメリットを比較する必要さえないため、ここでは必ずしもそのことを前提としないケースについて検討します）、前者の欠点は、アプリケーション全体において、トランザクション/ロックの要件が必要である場合には、それ自体単体で見れば悲観的ロックと比較して軽量な処理であるはずの楽観的ロックの実装が問題となりうることを示しています。

後者については、一概に結論付けられるようなものではなく、開発コスト（学習曲線）以外のアドバンテージなど諸々を総合的に考慮した上で、判断が必要になると思われますが、よく知られた、少なくない数のリレーショナルデータベースで、デフォルトの分離レベルとしてリードコミッテッドが採用されているのも事実です。

=== 性能を犠牲にしないトランザクション適用の柔軟性を持つCouchbase Server

NoSQLでは、当初、RDBのような複数レコードに跨がるトランザクションはサポートされていませんでした。上に見てきたように、今となっては、それも昔の話です。
一方、後天的にトランザクションが実装されたデータベースに対して度々見られる批判として、当初有していた性能面の利点が捨て去られ、データベースとして凡庸な位置づけに堕した、というものがあります。
トランザクションの実現とデータベースの性能は、論理的にトレードオフの関係であることは明白ですが、Couchbase Serverでは、リクエストベースで、トランザクションの適用を選択することができ、必要な箇所でのみトランザクションの恩恵を受けることができます。


====[column]関連情報紹介「How we implemented Distributed Multi-document ACID Transactions...」
より詳しい内容に興味がある方へ向けて、ブログ記事「How we implemented Distributed Multi-document ACID Transactions in Couchbase」@<fn>{distributed-multi-document-acid-transactions}を紹介します。

この記事では、RDBMSとNoSQLドキュメント指向データベース、そしてNewSQLのトランザクションを比較した上で、Couchbase Serverにおけるマルチドキュメント分散ACIDトランザクションがどのように実装されているかを解説しています。

結論部から、Couchbase Serverのトランザクションの優位性を列挙します。

 * Low Cost – Pay for what you use
 * No Central Transaction Manager or Central Coordinator
 * No Internal Global Clock and no MVCC(Multiversion Concurrency Control)
 * Flexibility

最後の「Flexibility」については、データベースによって一律に同じトランザクション管理方法が強制されるのではなく、アプリケーションの必要性に応じて、記事中で紹介されている様々な機能を使い分けることができることを指しています。

====[/column]



//footnote[distributed-transactions-multi-node-and-multi-bucket][https://docs.couchbase.com/server/current/learn/data/transactions.html#distributed-transactions-multi-node-and-multi-bucket]


//footnote[b0d8e878210f45b315d30ab3106b7a0e][https://blog.couchbase.com/couchbase-brings-distributed-multi-document-acid-transactions-to-nosql/]

//footnote[d2d87c2fb49681980479824d0e447353][https://blog.couchbase.com/distributed-multi-document-acid-transactions-in-couchbase/]


//footnote[distributed-multi-document-acid-transactions][https://blog.couchbase.com/distributed-multi-document-acid-transactions/]

== 第一正規化制約を超えて

=== NoSQLのカテゴリーについて

NoSQLデータベースの世界では、JSONを扱うデータベースは、「ドキュメント指向データベース（ドキュメントストア）」と呼び習わされてきました。「ドキュメント」は普通名詞としてあまりに一般的すぎるため、「準構造化データベース」とでも呼んだ方が、実態をよりよく表現する、ということができます。言い換えると、「ドキュメント指向データベースはドキュメント形式のデータを扱うが、ドキュメント形式のデータとは、JSONやXMLのような準構造化データである」という定義よりもよほどシンプルだと言えます。

=== 「準(半)構造化データ」という呼称について

「semi-structured data」という用語には、暗にstructured(構造)イコール、リレーショナルデータベースのテーブル形式、ということが含意されている、と解釈できそうです。
「構造」という言葉を素直に受け止めるのであれば、これはリレーショナルデータベースに対する行き過ぎた偏向であって、テーブル形式以外の方法でもデータは構造化することができ、JSONデータは、その意味で紛れもなく「構造」を持っているといえます。

上記でいうところの「テーブル形式」は、リレーショナルデータベースにデータを格納する際の制約である、第一正規化が行われた構造、ということができ、翻ってJSONデータについて、「準構造化データ」というよりも、「非第一正規形データ」と呼ぶ方が、より具体的な内実を表しているとも言えそうです。

=== Couchbase ServerとRDBのデータモデルの違いと共通点

Couchbase Serverには、スコープとコレクションという、RDBにおけるスキーマとテーブルに相当する概念が存在します。
Couchbase Serverのコレクションは、RDBにおけるテーブルのように、データの種類(エンティティ)を表現することができますが、RDBにおけるテーブルとは異なり、データ構造を制約するものではありません。

そして、RDBのテーブルに格納されるデータの条件である、第一正規形データは、準(半)構造化データに包含されます。

このように、Couchbase Serverは、データのエンティティに関する表現(コレクション)を持ち、テーブル(表)形式のデータを難なく扱えることから、RDBのデータ(モデル)をCouchbase Serverに移行(応用)することは、造作なく行えます。
そして、この関係は非可逆であると言えます。つまり、JSONは第一正規形データを表現することができ、また、スキーマレスであることは静的なスキーマ定義を問題視しませんが、その逆は成り立ちません。こうした点を踏まえ、「RDBのスーパーセットとしてのCouchbase Server」について考えてみることができます。

=== RDBのスーパーセットとしてのCouchbase Server

Couchbase Serverのクエリ言語には、N1QL (Non 1st normal form Query Language)という名称が付けられています。
N1QLでは、SQLをベースに、JSON固有の要素、配列やネストされたデータ構造に対して、構文が拡張されています。
視点を変えれば、JSON固有の要素が介在しない場合、RDBで使われているものと同じクエリを使うことができるといえます。

そうした面から見ると、先に検討したようなJSONの表現力の点のみではなく(それだけであれば、他のドキュメント指向データベースについても同じことが言える訳ですが)、N1QLを備えているからこそ、Couchbase ServerはRDBのスーパーセット足り得ている、という見方が成立します。このSQLが使えるという利点は、NoSQLデータベースとしては例外的ですが、NoSQL以降に登場したNewSQLデータベースには備わっている特徴です。そうした面から見ても、Couchbase Server(のデータベースとしての位置付け)は興味深いと言えるでしょう。

もっとも、扱うデータ型の種類について、JSONはSQL標準よりも限定されていること、Couchbase Serverの複数のドキュメントに跨がるトランザクションのトランザクション分離レベルは、一般のRDBよりも限定的であること、こういった諸々の違いは認識しておく必要があります。
にも関わらず、「スーパーセット」という挑発的な表現を用いたのには、NoSQLという名称が、RDBへのアンチテーゼとしての意味合いを持ち、NoSQLデータベース一般がそうしたものと目されている状況に対して、行き過ぎを正す意図があります。多くのNoSQLデータベースが、データ集約基盤やヒストリカルデータ分析等のようなRDBとは異なるユースケースで使われているのに対して、Couchbase Serverは、インタラクティブなアプリケーションのバックエンドデータベースとして、設計・最適化されており、これはRDBの主要なユースケースと共通しています。

NoSQLの様々なカテゴリーの中でも、ドキュメント指向データベースは、「そのデータ構造の柔軟性を生かしてアジャイル開発に用いられる」@<fn>{agile_memo}というケースを典型として、インタラクティブなアプリケーションとの親和性は、元々高かったと言えます。
一方で、現在のCouchbase Serverが備えている様々な特徴、つまり、メモリーファーストアーキテクチャーが実現するミリ秒以下の応答性能、SQLについての知識と経験を有効に活用できるN1QLクエリ、そしてマルチドキュメントトランザクションが実現する従来の範囲を超えたユースケース等、これらを踏まえて考えると、「ドキュメント形式のデータ」の「データ構造の柔軟性」からの発想は、ミスリーディングにさえなりかねません。@<fn>{agile_memo_two}

言い換えれば、アプリケーションが扱うデータ形式(この場合、JSONであること)からスタートして、採用するデータベースを決定するルートは、必ずしも必然ではない、ということです。
JSONのツリー構造は、より単純なデータ構造にも適用できるため、例えばレガシーシステムをモダナイズする場合のように、データ構造の柔軟性が必ずしも必要でなく、アジャイル開発が適用されないプロジェクトを含め、Couchbase Serverを活用することのできる場面は多岐に渡って存在します。

//footnote[agile_memo][さらに言えば、「厳格なデータ設計プロセスをオミットして開発を始める」という表現も可能でしょう。]

//footnote[agile_memo_two][特に、先の脚注に引いた表現と組み合わされる場合には。]

//footnote[meti_go_jp_dx][https://www.meti.go.jp/press/2019/07/20190731003/20190731003-1.pdf]

//footnote[1433ffe25ca932a2c5c0242cfc86b3e3][https://www.cdata.com/drivers/couchbase/]


//footnote[dee7910fc6a7f7780ac7831f794eda19][https://coherence.community/]

//footnote[522195171ed14f78e1f33f84a98f0de6][https://redis.io/]

//footnote[89abbd125f9fb071d8e485290dacbc26][https://www.cdata.com/jp/kb/tech/redis-sync-mongodb.rst]

//footnote[9a6cce3336b02e30ca1fb60d9b70e9ad][https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e]

//footnote[1a4091951f5fb1f307e2ee166df63c1d][https://en.wikipedia.org/wiki/Ajax_(programming)]

//footnote[4cbf4c2bb4d7602315f6b0d7af6745be][https://en.wikipedia.org/wiki/Single-page_application]

//footnote[7421ae409426b8fcd3cb845fe195a411][https://en.wikipedia.org/wiki/Progressive_web_application]



//footnote[66fe5150939f305857c0310ca092a9bc][https://jpn.nec.com/infoframe/relationalstore/docs/irs_couchbase_live_tokyo_2015.pdf]

//footnote[90234a6d1bc62d7f4d443da72a796c21][https://www.infoq.com/news/2011/04/newsql/]

//footnote[f330266e643fbd2b7d2a10960b35a423][https://cloud.google.com/spanner]

//footnote[google-spanner][https://research.google/pubs/pub39966/]

//footnote[d6032cdd340139ba8d35f5aec7d0d61e][https://www.cockroachlabs.com/]

//footnote[d9883eb51d84bd0f4bfe3bafca1931b5][https://pingcap.com/]

//footnote[e968a7936c71fff2010123dc01af46ab][https://www.yugabyte.com/]


