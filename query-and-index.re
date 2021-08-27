= クエリとインデックス

====[column]
Couchbase Serverでは、RDBとは異なり、クエリの実行にあたり対応するインデックスが存在していることが前提となります。この点を踏まえ、ありうべき混乱を避けるため、本章では、まずインデックスについて解説し、その後、クエリについて解説します。

====[/column]

== インデックス

Couchbase Serverにおける、インデックス機能の基本をRDBとの比較を交えながら説明します。

=== 用語法に関する注釈

Couchbase Serverにおけるインデックスは、グローバルセカンダリーインデックス（GSI）と呼ばれています。別に「プライマリインデックス」、「セカンダリーインデックス」という表現も用いられますが、これらはいずれも、グローバルセカンダリーインデックス（GSI）の中のカテゴリーとなります。（以下は、「プライマリインデックス」のメタデータの例です。@<tt>{"is_primary": true}かつ @<tt>{"using": "gsi"} であることが分かります）

//emlist{
{
  "datastore_id": "http://127.0.0.1:8091",
  "id": "1c23ecbf1f7cd99e",
  "index_key": [],
  "is_primary": true,
  "keyspace_id": "travel-sample",
  "name": "#primary",
  "namespace_id": "default",
  "state": "online",
  "using": "gsi"
}
//}

=== 来歴

グローバルセカンダリーインデックス（GSI）が登場する前には、データの検索のために、MapReduceビュー（View）という機能が使われていました（現在も互換性のために維持されています）。このViewがデータノードに対して「ローカル」であったことに対して、データノードのローカリティに依存しない、クラスター単位で管理されるインデックスという意味で、「グローバル」という表現が用いられていると解釈することができます。「セカンダリー」という形容には、RDBにおけるセカンダリーインデックスのように任意の数のインデックスを定義できる、というニュアンスで受け止めることができます。

====[column]
Couchbase Serverには、ローカルセカンダリーインデックス(Local Secondary Index)という概念は存在しません。

====[/column]

====[column] 過去のバージョンとの違い
Couchbase Serverの過去のバージョンではGSIとしてイ ンデックスを作成する際に明示的に@<tt>{USING GSI}という句が用いられていました。6.5以降のバージョンでは、 @<tt>{USING GSI}の利用はオプションであり、省略可能です。

====[/column]

=== RDBのインデックスとの違い

Couchbase ServerのSGIと、RDBとのインデックスの間には、ユーザーが意識しておくべき、下記のような違いがあります。

 * Couchbase Serverにおけるクエリでは、そのクエリを実行するために必要なインデックスが存在しない場合、処理が失敗します。
 * インデックスは、非同期で維持されます。
 * 同じ内容のインデックスを別名で複数定義できます。

最後の点について、完全に同じ内容のインデックスを別名で、つまり複数、作成することを許していることにどのような意味があるでしょうか？　Couchbase Serverのインデックスは、RDB/SQLの構文を可能な限り踏襲しながら、RDBにはない、ユーザーによる様々な制御の余地を提供しています。例えば、これは分散アーキテクチャー独自の部分になりますが、インデックス定義時に、ローカリティを指定することが可能です。同じ内容のインデックスを別名で、複数定義できるという特徴と組み合わせて、複数のノードに同じインデックスを格納することによって、クエリの性能を向上するという使い方ができます。

その他の特徴についても、それぞれ関連する章で、背景やその特徴を踏まえた利用方法を説明しています。

=== ストレージモデル

Couchbase Serverのグローバルセカンダリインデックス(GSI)には以下の2つのストレージモデルがあります。

 * スタンダードGSI
 * メモリ最適化(Memory-optimized)GSI

====[column]
メモリ最適化GSIは、コミュニティエディショ ンでは利用することができません。

====[/column]

これらは共に、Multi Version Concurrency Control (MVCC, マルチバージョン・コンカレンシー・コントロール)を実装しており、一貫したインデックススキャン結果と高スループットを提供します。

スタンダードグローバルセカンダリインデックスは、ForestDBストレージエンジンを使用して、Bツリーインデックスを格納しています。また、最適なワーキングセットのデータをバッファに保持します。

メモリ最適化インデックスは、すべてのインデックスデータをメモリに持ち、ロックフリースキップリストを使用してインデックスを保持しています。メモリ最適化インデックスは、データの変更に際して、に高速に処理が行われます。

====[column]
スタンダードGSI、メモリ最適化GSIのいずれを使うか は、クラスター単位の設定となり、クラスターの初期構成時に、そのクラスターで使用するインデックスのタイプを選択することになります。

====[/column]

=== プライマリインデックス

プライマリインデックスは、バケット単位で指定するインデックスです。

====[column]
Couchbase Serverでは、ここで説明するプライマリインデックスとは関係なく、ドキュメントキーには一意制約があります。

====[/column]

プライマリインデックスが使用されるケースとしては、以下があります。

 * クエリにフィルタ（述語・WHERE句）がない場合
 * クエリにフィルタ（述語・WHERE句）があるが、該当するセカンダリーグローバルインデックスがない場合

Couchbase Serverでは、クエリを実行するために必要なインデックスが存在しない場合、処理が失敗するという性質があります。
プライマリインデックスは、ドキュメントの特定のフィールドに限られない、キースペース（下記例の場合指定された@<tt>{airline}コレクション）全体に対して、インデックスを作成することができるため、そのような制約を回避するための最も簡便な方法になります。つまり、クエリの検索条件に使われている項目を個別に指定してインデックスを作成しなくても、クエリを実行することが可能になります。とはいえ、結合を行う際など、プライマリインデックスで代用できない場合があります。

//emlist{
CREATE PRIMARY INDEX ON `travel-sample`.inventory.airline;
//}

作成されたインデックスの定義（ @<tt>{Definition}）を確認すると分かりますが、上記のDDLは、下記のDDLを実行する際の糖衣構文(syntax sugar)にあたるものです。(Couchbase Serverのインデックスには名前を定義するのが通常であり、上記の省略形は、プライマリインデックスに特有のものです)

//emlist{
CREATE PRIMARY INDEX `#primary` ON `travel-sample`.inventory.airline
//}

したがって当然、次の例に示すように、明示的に、名前を付けて定義することもできます。

//emlist{
CREATE PRIMARY INDEX <インデックス名> ON <バケット名>;
//}

====[column]
プライマリインデックスによる検索は、検索条件の内容に関わらず、対象キースペース（上記例の場合指定された@<tt>{airline}コレクション）に対する全件スキャンを実行することに留意が必要です。これは、ドキュメントキーによる検索の場合も同様です（ドキュメントキーを用いたセカンダリーインデックスの定義については別に触れます）。実際上、プライマリインデックスは、開発時の利便性のためのものであると捉えることができます。実運用においては、プライマリインデックス定義を用いないことがベストプラクティスとなります。

====[/column]

====[column]
アドホックなクエリのために、（本書の別の箇所で紹介している）アダプティブ（Adaptive）インデックスの利用を検討することができます。アダプティブインデックスの利用により、プライマリーインデックスの特徴である全件スキャンと、それに伴う検索性能劣化、を避けることができます。ただし、アダプティブインデックスによってインデックス定義の自由度は増す事になりますが、それでも検索対象フィールドを特定する必要はあり、インデックス作成時に考慮されていなかったフィールドを用いて検索することもできる、プライマリインデックスのと見なすことができる訳ではありません。

====[/column]

=== セカンダリインデックス

JSONドキュメントの特定の要素（フィールドまたはドキュメントのキー）に対して設定するインデックスは、（バケット単位で定義されるインデックスであるプライマリインデックスと区別して）セカンダリインデックスと呼ばれています。

まずは以下、JSONのフィールドの扱いの違いによる定義方法を見ていきます。

==== トップレベルフィールド

//emlist{
CREATE INDEX travel_name ON `travel-sample`.inventory.airline(name);
//}

このキースペースにおいて、@<tt>{name} は次のような単純なスカラー値です。

//emlist{
{ "name": "Air France" }
//}

述語条件の左項は、スカラー値のみではなく、JSONの他のデータ形式（配列、オブジェクト）を用いることも可能ですが、この場合、述語条件の右側の値は同様に構造化されたデータ形式（配列、オブジェクト）である必要があります。

==== ネストされたオブジェクト

ドット(@<tt>{.})表記（ノーテーション）により、ネストされたオブジェクト（のフィールド）を指定することができます。

//emlist{
CREATE INDEX travel_geo_alt on `travel-sample`.inventory.landmark(geo.alt);
CREATE INDEX travel_geo_lat on `travel-sample`.inventory.landmark(geo.lat);
//}


@<tt>{geo} は、次のようなドキュメント内に埋め込まれたオブジェクトです。

//emlist{
"geo": {
  "alt": 12,
  "lat": 50.962097,
  "lon": 1.954764
}
//}

=== 複合（コンポジット）インデックス

複数のキーを持つインデックスを定義することが可能です。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(name, id, icao, iata);
//}

=== ドキュメントキーのインデックス

セカンダリインデックスのキーとして、ドキュメントキーを用いることができます。ドキュメントキーは、ドキュメント内の情報ではないため、　@<tt>{meta()}　キーワードを用います。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(meta().id);
//}

=== インデックスのメタデータ

@<tt>{system:indexes}に対してクエリすることにより、インデックスのメタデータを確認することができます。

==== 利用例

//emlist{
SELECT * FROM system:indexes WHERE name = '#primary';
//}

//emlist{
[
  {
    "indexes": {
      "bucket_id": "travel-sample",
      "datastore_id": "http://127.0.0.1:8091",
      "id": "804d33907ce31e27",
      "index_key": [],
      "is_primary": true,
      "keyspace_id": "airline",
      "name": "#primary",
      "namespace_id": "default",
      "scope_id": "inventory",
      "state": "online",
      "using": "gsi"
    }
  }
]
//}

メタデータは、インデックスが存在するノード（@<tt>{datastore_id}）、状態（@<tt>{state}）など、インデックスに関する情報を提供します。


====[column] エディションによる差異

Indexサービスにおける、コミュニティエディションには含まれない、エンタープライズエディションの機能として、以下があります。

 * インデックスのパーティション化
 * インデックスレプリカ
 * メモリ最適化インデックスストレージ
 * Plasma（インデックス用の高速ストレージエンジン）
 * インデックスアドバイザー

コミュニティエディションユーザーは、右のサイトを使って、インデックスアドバイザー機能を利用することができます：https://index-advisor.couchbase.com/indexadvisor 
コミュニティエディションでは、WebコンソールのQuery Workbench画面に、「External Query Advisor」という上記サイトへのリンクが表示されます。

====[/column]

==== 参考情報

Couchbase公式ドキュメント Using Indexes@<fn>{global-secondary-indexes}

//footnote[global-secondary-indexes][https://docs.couchbase.com/server/current/learn/services-and-indexes/indexes/global-secondary-indexes.html]

== インデックス設計

ここで解説する内容を理解することにより、Couchbase Serverのクエリのパフォーマンスを向上させることができます。

=== カバリングインデックス

クエリがインデックス内のキーのみを参照している場合、クエリエンジンは、データノードからフェッチすることなく、インデックススキャン結果のみを利用してクエリに応答することができます。

検索に用いる必要のないキーをあえてインデックスに含めて定義することで、特定のクエリの性能を向上することができる一方、インデックスの維持に必要なリソース負担が増します。性能とリソースのバランスを考慮して最適化することになります。

=== パーシャル（部分）インデックス

@<tt>{CREATE INDEX} DDLの定義に @<tt>{WHERE}句を用いることで、本来のデータに対して、部分的にインデックスを作成することができます。

//emlist{
CREATE INDEX travel_eat ON `travel-sample`.inventory.landmark(name, id, address)
WHERE activity='eat';
//}

上記例では、@<tt>{activity} フィールドの値として @<tt>{eat}を持つ、ドキュメントに対してのみインデックスが作成されます。
このインデックスがクエリに利用されるためは、そのクエリにも同じ条件が含まれている必要があります。

インデックスの @<tt>{WHERE} 句では、より複雑な述語を使用できます。

パーシャルインデックスを使用する目的として以下が考えられます。

 * 必要な検索要件のみに特化したインデックス化範囲の限定による（更新・検索）性能の最適化
 * 検索の選択肢が限定的な場合（性別や地域名など）に、複数のインデックスとして定義することによる性能の最適化（この場合、インデックス定義時のノード配置の指定と組み合わせて、分散環境の効果を活用することが考えられます）

=== 関数(Functional)インデックス

関数を適用したフィールドをキーとして使用しインデックスを作成することができます。
インデックスには、関数適用後の値が格納されます。ただし、複数ドキュメントに跨がる集計関数 (@<tt>{SUM}、@<tt>{AVG}、@<tt>{COUNT} 等)は使用できません。

//emlist{
CREATE INDEX travel_cxname ON `travel-sample`.inventory.airport(LOWER(name));
//}

上の例は、大文字と小文字が混在している可能性のある @<tt>{name}フィールドを検索するケースを想定しています。
大文字と小文字の組み合わせの全ての可能性を検索するために、 @<tt>{LOWER}関数を使って値を小文字に統一しています。

このインデックスがクエリに利用されるためは、下のように、条件式の左辺、関数 @<tt>{LOWER}と関数へのパラメータ @<tt>{name} の組み合わせ(@<tt>{LOWER(name)})、がインデックスの定義と一致している必要があります。

//emlist{
SELECT * FROM `travel-sample`.inventory.airport
WHERE LOWER(name) = "villeneuve-sur-lot";
//}

この例では、条件式の右辺に、値が小文字で与えられています（念のために言えば、インデックスとの一致のために、条件式の右辺に@<tt>{LOWER}関数を使用することは必要ありません）。

より複雑な式の組み合わせを使用することもできます。例えば以下のように、計算式を指定することも可能です。

//emlist{
CREATE INDEX travel_cx1 ON `travel-sample`.inventory.airport
(LOWER(name), ROUND(geo.alt * 0.3048));
//}

=== 重複(Duplicate)インデックス

異なる名前で、名前以外は、共通した定義を持つインデックスを重複して定義することは許されています。

重複インデックスは、高可用性と負荷分散に役立ちます。

//emlist{
CREATE INDEX i1 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.1.0:8091"]};

CREATE INDEX i2 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.2.0:8091"]};

CREATE INDEX i3 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.3.0:8091"]};
//}

上の例では、3つのインデックスはすべて、同じキーと同じWHERE句を持っています。唯一の違いは、これらのインデックスの名前です。
@<tt>{CREATE INDEX} ステートメントの @<tt>{WITH}句を使用して、物理的な場所を指定します。(指定しない場合、ランダムに決定されます)

クエリの実行時、最適化の一貫として、重複インデックスはラウンドロビン方式で使用され、負荷が分散されます。

エンタープライズエディションでは、下記のように複数のノード指定を行うことができます。

//emlist{
CREATE INDEX iDuplicate ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WITH {"nodes":["node1:8091", "node2:8091", "node3:8091"]};
//}

====[column] @<tt>{WITH}句のその他の利用方法 
本書では詳細は触れませんが、@<tt>{WITH} 句には、@<tt>{node}の他に、@<tt>{num_replica}（インデックスレ プリカの数）、そして真偽値をとる@<tt>{defer_build}（インデックスの遅延作成）があります。

====[/column]

=== 配列インデックス

JSONは、スカラー、オブジェクト、または配列をフィールドとして持つことができます。各オブジェクトは、他のオブジェクトや配列をネストできます。各配列は他のオブジェクトや配列を持つことができます。

Couchbase Serverのインデックス定義DDLは、配列に対してインデックスを作成するための、特別な構文を備えています。

具体的に理解するため、次の配列について考えてみます。

//emlist{
"schedule": [
  {
    "day": 0,
    "flight": "AF198",
    "utc": "10:13:00"
  },
  {
    "day": 0,
    "flight": "AF547",
    "utc": "19:14:00"
  },
  {
    "day": 0,
    "flight": "AF943",
    "utc": "01:31:00"
  },
  {
    "day": 1,
    "flight": "AF356",
    "utc": "12:40:00"
  },
  {
    "day": 1,
    "flight": "AF480",
    "utc": "08:58:00"
  },
  {
    "day": 1,
    "flight": "AF250",
    "utc": "12:59:00"
  }
]
//}

@<tt>{schedule}
配列に見られるような複雑な構造に対して、サブオブジェクト内の特定の配列またはフィールドにインデックスを付ける方法を次に示します。

//emlist{
CREATE INDEX idx_sched
ON `travel-sample`.inventory.route
( DISTINCT ARRAY v.flight FOR v IN schedule END );
//}

上のインデックス定義DDLは、以下の基本構文を元にしています。。

//emlist{
ARRAY <インデックスキー> FOR v IN <配列名> END
//}

@<tt>{v} は、@<tt>{schedule}配列内の各要素/オブジェクトを参照するように暗黙的に宣言された変数です。@<tt>{v.day}は、配列の各要素であるオブジェクトの @<tt>{day} フィールドを示しています。

これは一般化された式であるため、インデックスを作成する前にデータに追加のロジックと処理を適用する柔軟性を提供します。たとえば、各配列の要素に関数型インデックスを適用できます。

@<tt>{DISTINCT}修飾子により、インデックスからはキーの重複が除かれます。全てのキーを格納する場合は@<tt>{ALL} 修飾子を利用します。

上記のインデックスにより、下記のように @<tt>{schedule.flight}に対して検索を行うことが可能になります。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE ANY v IN schedule SATISFIES v.flight LIKE 'UA%' END;
//}

@<tt>{ARRAY <インデックスキー> FOR v IN <配列名> END}構文と合わせて利用することのできるキーワードを使って、複雑な条件指定を行うことができます。

==== 配列から特定の条件で選択された要素の全てのフィールドを使ってインデックスを作成する例

週の最初の4日間にスケジュールされたサンフランシスコからのフライトのインデックスを作成します。

//emlist{
CREATE INDEX idx_flight_sfo
ON `travel-sample`.inventory.route
( ALL ARRAY v.flight FOR v IN schedule WHEN v.day < 4 END )
WHERE sourceairport = "SFO";
//}

週初日にスケジュールされた「UA」フライトのリストを検索します。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE sourceairport = "SFO" ①
AND ANY v IN schedule SATISFIES (v.flight LIKE 'UA%') ②
AND (v.day=1) END; ③
//}

上のインデックスは、次のような関係において、上のクエリの対象となります。

① クエリ述語中の @<tt>{sourceairport = "SFO"} の部分は、インデックスの @<tt>{WHERE}句に一致します。

② クエリ述語中 @<tt>{ANY-SATISFIES} 句の ` schedule`配列に関する検索のキーとして、インデックスキー @<tt>{v.flight} が一致します。

③ クエリ述語中 @<tt>{ANY-SATISFIES} 句のもう一つの条件 @<tt>{v.day=1}は、インデックス定義の @<tt>{WHEN} 句の条件「 @<tt>{v.day < 4} 」に包含されます。

==== 配列の全ての要素から選択されたフィールドを使ってインデックスを作成する例

フライトスケジュールのフライト番号とフライト日のみを使ってインデックスを作成します。

//emlist{
CREATE INDEX idx_flight_day ON `travel-sample`.inventory.route
    (DISTINCT ARRAY [v.flight, v.day] FOR v IN schedule END);
//}

2日にスケジュールされた「US681」フライトのリストを検索します。

//emlist{
SELECT meta().id FROM `travel-sample`.inventory.route
WHERE ANY v in schedule SATISFIES [v.flight, v.day] = ["US681", 2] END;
//}

==== 簡略化された構文を使用して配列内のすべての要素からインデックスを作成する例

スケジュールのインデックスを作成します。

//emlist{
CREATE INDEX idx_sched_simple
ON `travel-sample`.inventory.route (ALL schedule);
//}

特定のスケジュールに一致するすべてのルートを検索します

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE ANY v IN schedule
SATISFIES v = {"day":2, "flight": "US681", "utc": "19:20:00"} END; 
//}

スケジュール配列の要素はオブジェクトであるため、述語条件の右側の値は同様にオブジェクトである必要があります。

N1QLがもつ、配列操作のためのキーワード @<tt>{UNNEST}を使って、上のクエリを下記のように書き換えることもできます。

//emlist{
SELECT * FROM `travel-sample`.inventory.route t
UNNEST schedule sch
WHERE sch = {"day":2, "flight": "US681", "utc": "19:20:00"};
//}

=== アダプティブ（Adaptive）インデックス

@<tt>{DISTINCT PAIRS}構文を利用して、ドキュメントの指定されたフィールドに対して、インデックスを付けることができます。指定されたフィールドの順序にかかわらず、全ての組み合わせを用いてクエリを実行することが可能になります。

これにより、フィールドのさまざまな組み合わせに対してさまざまな複合インデックスを作成しなくても、効率的なアドホッククエリが可能になります。

これは、特殊なタイプの配列インデックスです。

//emlist{
CREATE INDEX `ai_airport_day_faa`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS({airportname, city, faa}));
//}

さらに`self`キーワードを用いることにより、全てのフィールドを対象とすることができます。

//emlist{
CREATE INDEX `ai_self`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS(self));
//}

=== 性能最適化における考慮点

==== カーディナリティ

一般にデータベースにおけるインデックススキャンについては、カーディナリティが高い（データの種類が多い）項目の検索に向いているということが言えます。これは、カーディナリティが低い（データの種類が少ない）項目の検索では、実際上、フルスキャンに違い動きになる（フルスキャンが向いている）ことと裏返しの関係になります。

Couchbase Serverのインデックスにおいても、属性リストの先頭に高いカーディナリティの属性を配置することで、インデックスの検索性能を最適化することができます。ただし、ここで重要なのは、先行属性(leading attributes)の一致を妨げない限り、という条件が付くことです。

クエリに一致するインデックスの決定は、以下の順序で行われます。

①　キースペース(バケット名)マッチ

②　条件句マッチ

③　先行属性（leading attributes）マッチ

具体的には、クエリ中の下記の要素が対応します。

//emlist{
SELECT *
FROM retail.east.customer c ①
WHERE c.age = $age　③
AND c.grade = "premium"　②
//}

//emlist{
CREATE INDEX idx_cust ON retail.east.customer(age, sex)
WHERE grade ="premium"
//}

上記のクエリとインデックスの例は、先行属性マッチとカーディナリティの関係が適切な例となります。

ただし、下記のようなクエリでは、上のインデックスを用いることができません。

//emlist{
SELECT *
FROM retail.east.customer c 
WHERE c.sex = $sex
AND c.grade = "premium"　
//}

=== 複数ドキュメント取得時の最適化

クエリにより、多数の(かつ大きな)ドキュメントが返される場合は、クエリでドキュメントキーをクライアントに返し、クライアントからK-V API を使用してドキュメントをフェッチすることができます。
すべてのデータ転送を実現するために、クエリプロセスのメモリ領域がボトルネックとなる事がありません。また、1つの大きなネットワーク伝送ではなく、多数の小さなネットワーク伝送を用いることが可能になります（ドキュメントキーによるデータ取得は、そのドキュメントを管理しているノードから直接行われます）。

//emlist{
CREATE INDEX idx_order_pending ON retail.east.order(META().id)
WHERE status = "Pending"
AND doc.type = "order";
//}

//emlist{
SELECT META().id as docKey
FROM retail.east.order
WHERE META().id IS NOT MISSING
AND status = "Pending"
//}

ここでは、@<tt>{META().id} を検索の目的でなく、クエリ性能最適化のために利用しているため、カバリングインデックスの例ということができます。
このクエリでは、@<tt>{META().id}を持たないドキュメントは存在しませんが、@<tt>{META().id IS NOT MISSING}という条件は、インデックスとの一致（先行属性マッチ）のために必要になります。

== N1QLクエリ

Couchbase ServerのN1QLクエリについて、SQLとの差分を中心に解説します。

====[column]
SQL標準を含めた網羅的な解説であることは意図されていません。

====[/column]

=== 基本

==== エスケープ文字

ユーザー定義語句（バケット名、フィールド名）は、「`」(バッククォート)を使ってエスケープすることによって、予約語との衝突を回避することができます。
Couchbase Serverでは、「-」は、予約語に含まれるため、「-」を含むバケット名を利用する場合、下記のようにバケット名をエスケープする必要があります。

//emlist{
SELECT ... FROM `travel-sample`
//}

=== 構造

N1QLは、JSONデータを構成する文字列、数値、真偽値を（直接的に、つまり文字列としてエスケープされた形ではなく）含むことができます。以下に具体的な例を見てみます。

==== コンストラクションオペレーター

コンストラクションオペレーターと呼ばれる次の記号をクエリ内で直接用いることができます。

@<tt>{{} @<tt>{\}} @<tt>{:} @<tt>{[} @<tt>{]}

==== ネステッドオペレーター

ネステッドオペレーターと呼ばれる以下の表現を利用することができます。

 * ドットノーテーション （@<tt>{.}）:
ネストされたオブジェクト（サブドキュメント）へのパスをドット記号（@<tt>{.}）を使って表現できます。

 * 配列表現（インデックス、スライス）:配列へのアクセスに、インデックス（添字）や、スライス（範囲による指定）を利用することができます。

=== ドキュメントキーの利用

==== @<tt>{USE KEYS}

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS "airport_1254";
//}

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS ["airport_1254","airport_1255"];
//}

@<tt>{USE KEYS}のみを利用してクエリを行う場合、インデックスを作成する必要がありません。

====[column]
Couchbase Serverでは、ドキュメントキーが判明している場合には、ドキュメントキーを指定してドキュメントを取り出すことが、（RDBにおける 、主キーでの検索のように）最も効果的であることは間違いありません。一方でドキュメントキーが判明しているケースにおいては、性能を追求する場合、必ずしもクエリ用いて(Queryサービスを介して）ドキュメントを取り出す必要はなく、Dataサービスに対して直接リクエストすることで性能を最適化することが可能です（特に上記の例のような単純な利用の場合。一方、サブクエリを伴うような複雑なクエリの場合など、@<tt>{USE KEYS}句を効果的に利用できる場面があります）。

====[/column]

==== @<tt>{META().id}

上記の@<tt>{USE KEYS}を利用したクエリは、下記のように @<tt>{META().id}
を使って書き直すことができます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
WHERE META().id IN ["airport_1254","airport_1255"];
//}

@<tt>{META().id} を利用してクエリを行う場合、@<tt>{META().id}を用いたインデックスが作成されている必要があります。

@<tt>{USE KEYS}の構文では、後続する鉤括弧の中のドキュメントキーは完全一致が想定されていますが、@<tt>{META().id} を利用する場合、N1QLクエリの一部として、@<tt>{LIKE}と組み合わせることができ部分一致検索を行うことができる他、比較演算子による大小比較など、クエリの名で用いる際により自由度が高い表現と言えます。

また、検索条件としてのみではなく、 @<tt>{SELECT META().id FROM ... WHERE ...}のように、条件に一致するドキュメントキーを検索するために用いることもできます。

上記のクエリ中の@<tt>{META().id} の部分は、下記のようなキースペース(の別名)を含んだ表現(@<tt>{META(a).id} )を用いることができます。このクエリのように、クエリの中でキースペースが一意に決まっている場合、指定は必須ではありません。複数のキースペースが混在している中で、ドキュメントのキーを用いる（取り出す）場合には、キースペースを指定した表現を用います。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport a
WHERE META(a).id IN ["airport_1254","airport_1255"];
//}

=== フィールドから値のみを取り出す: @<tt>{RAW} | @<tt>{ELEMENT} | @<tt>{VALUE}

@<tt>{RAW},@<tt>{ELEMENT},@<tt>{VALUE}, これらは全て同義語です（以下では、@<tt>{RAW}を用います）。

N1QLで検索された結果は、JSONデータとして、フィールド名と値のペアとなることが基本ですが、@<tt>{RAW} キーワードを @<tt>{SELECT}句と共に用いることで、フィールドの値のみを取り出すことができます。

==== @<tt>{RAW} を用いない例

//emlist{
SELECT city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

結果は、JSONデータ（フィールド名と値のペア）の配列になります。

//emlist{
[
  {
    "city": "Abbeville"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Abilene"
  }
]
//}

==== @<tt>{RAW} を用いた例


//emlist{
SELECT RAW city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

結果は、値の配列になります。

//emlist{
[
  "Abbeville",
  "Aberdeen",
  "Aberdeen",
  "Aberdeen",
  "Abilene"
]
//}

==== @<tt>{DISTINCT} と組み合わせた利用例

//emlist{
SELECT DISTINCT RAW city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

結果は、値の重複しない配列になります。

//emlist{
[
  "Abbeville",
  "Aberdeen",
  "Abilene",
  "Adak Island",
  "Addison"
]
//}


=== 配列からデータを取り出す

@<tt>{ARRAY} または @<tt>{FIRST} から始まり、@<tt>{END} で基本単位となります。@<tt>{IN}または @<tt>{WITHIN} と @<tt>{FOR}により、コレクションの要素へのループを表現しています。

この構文を利用して、ドキュメントに含まれる配列が要素として、オブジェクト（サブドキュメント）を持っている時、そのオブジェクトのフィールドを配列として取り出すことができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　END
//}

==== @<tt>{ARRAY}

配列の要素全体を扱います（配列の要素に対して @<tt>{map} 操作を実行します）。

==== @<tt>{FIRST}

@<tt>{ARRAY} を @<tt>{FIRST}に置き換えることで、配列のはじめの要素のみを扱います。

==== @<tt>{WHEN}

@<tt>{WHEN} 句により、条件指定を付け加えることが可能です。配列の要素に対して@<tt>{filter} 操作を実行します。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　[ ( WHEN cond1 [ AND cond2 ] ) ] END
//}

下記の通り、上記説明した要素の組み合わせにより複雑なステートメント用いることができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　
[ ,var2 ( IN | WITHIN ) expr2 ]*
[ ( WHEN cond1 [ AND cond2 ] ) ] END
//}

=== 配列を検索条件に用いる

@<tt>{ANY}　または @<tt>{EVERY} から始まり、@<tt>{END}までで基本単位となります。@<tt>{SATISFIES} で条件を指定します。

//emlist{
( ANY | EVERY ) var1 ( IN | WITHIN ) expr1
[ , var2 ( IN | WITHIN ) expr2 ]*
SATISFIES condition END
//}

====[column]
この構文は、配列から、真となった要素を取り出しているのではないことに注意してください。これは、SQLの WHERE条件が検索に使ったカラムを取 り出すものではないのと同様です。

====[/column]

==== @<tt>{ANY}

配列に条件で指定した要素が一つでも含まれる場合、真となります。

//emlist{
SELECT *
FROM retail.east.order o
WHERE ANY item IN o.lineItems SATISFIES
item.count >= 5 END
//}

==== @<tt>{EVERY}

配列の全ての要素が、指定した条件に一致する場合、真となります。

//emlist{
SELECT *
FROM retail.east.order o
WHERE EVERY item IN o.lineItems SATISFIES
item.count >= 5 END
//}

==== @<tt>{IN} または @<tt>{WITHIN}

@<tt>{IN} 句が、指定された配列のトップレベルの要素を検索するのに対して、@<tt>{WITHIN} 句は、現在の配列とその子、および子孫を含めて検索対象とします。

=== ドキュメントキーによる結合: @<tt>{NEST}

@<tt>{NEST} は、外部の子ドキュメントを親の下に埋め込む特別なタイプの @<tt>{JOIN}です。
ドキュメント・キーを介した参照関係でデータをモデル化した際に用いる事ができます。@<tt>{ON KEYS}により、対象とするドキュメントのキーを指定することができます。


以下の2つのコレクションとドキュメントに対するクエリについて考えてみます(データの構造および内容は、あくまでこの箇所での説明のためのものであり、他の箇所と同じでないことにご注意ください)。

コレクション：@<tt>{retail.east.order}

//emlist{
{
"order_id":1234,
"customer_id":"34567",
"total_price":"65.5",
"lineitems":["o11","o12","o13"]
}
//}

コレクション：@<tt>{retail.east.lineItem}

//emlist{
{
"lineitem_id":o11,
"item_id":"789",
"qty":"3",
"itemprice":"5.99",
"base_price":"17.97",
"tax":"0.75",
"total_price":"18.22"
}
//}

//emlist{
{
"lineitem_id":o12,
"item_id":"234",
"qty":"5",
"itemprice":"10.00",
"base_price":"50.00",
"tax":"0.75",
"total_price":"50.75"
}
//}

//emlist{
SELECT ordr.order_id,
ARRAY {“item_id”: l.item_id, “quantity”:l.qty} FOR l IN line END as items
FROM retailsample.east.order ordr
NEST retailsample.east.lineItem line
ON KEYS ordr.lineitems
//}

結果は、下記のように、一つのドキュメントになります。

//emlist{
[
{
"items":[
    {"item_id":"789", "qty":"3"\},
    {"item_id":"234", "qty":"5"\}
  ];
  "order_id":"1234"
},
{
  "items":[
    {"item_id":"899", "qty":"8"\},
    {"item_id":"651", "qty":"2"\}
  ];
  "order_id":"9812"
}
]
//}

=== サブドキュメントのフラット化: @<tt>{UNNEST}

@<tt>{UNNEST}は、ネストされたオブジェクトを最上位ドキュメントとして表示するために用います。

下記のようなネストされた構造を持つデータを想定します（受注明細データを中に含む受注伝票データ）。

//emlist{
{
"ordId": "ORDER-0001",
"status": "Shipped",
"items": [
  {
  "prodId": "AAA-222",
  "qty": 1
  },
  {
  "prodId": "BBB-333",
  "qty": 2
  },
  {
  "prodId": "CCC-444",
  "qty": 3
  }
]
}
//}

@<tt>{UNNEST}は、このようなネストされた（サブドキュメントを含む）ドキュメントへのクエリの結果を下記のようなテーブル構造として利用したい時に使われます。

//table{
ordId	Status	prodId	qty
----------------------------
ORDER-0001	Shipped	AAA-222	1
ORDER-0001	Shipped	BBB-333	2
ORDER-0001	Shipped	CCC-444	3
//}


クエリでは、@<tt>{UNNEST} をサブドキュメントに対して指定し、@<tt>{as}で別名をつけたものを @<tt>{SELECT} 句の中で使用します。

//emlist{
SELECT ord.ordId, ord.status, item.* FROM retail.east.order ord UNNEST items as item
//}

下記のようなフラットな構造のJSONが取り出されます。

//emlist{
[
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “AAA-222”, “qty”: 1 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “BBB-333”, “qty”: 2 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “CCC-444”, “qty”: 3 }
]
//}

(JSON中の要素の出現順はこの通りではありません)

====[column]
JSONオブジェクトのフィールドは並び順に意味を持ちません。

====[/column]


=== データ型: @<tt>{NULL}, @<tt>{MISSING}


SQL利用者にとって、「NULL」という予約語や、@<tt>{IS [NOT] NULL}という比較演算は、馴染みがあるところです。
NULLは、データが存在しないことである、と言ったりします。より正確には、値が存在しない状態を表しています。ここで、データと値の違いはどこにあり、なぜ後者がより正確なのでしょうか？「データ」という表現は曖昧に取れる一方、「値」という言葉には、数学でいうところのXの値という使い方に見られるように（もちろん、プログラミングでいうところの、変数の値、でもいいわけですが）、単にそれ自体で存在しているデータを超えた含意があります。データベースのコンテクストでいえば、テーブルスキーマにおけるカラムは、一定のデータ型を持ちますが、数値型であれば0、文字列型であれば空文字（長さ0の文字）のようなデータ型固有の表現とは別に、値そのものが存在していない状態がNULLで表現されます。
JSONにおいても、「フィールドの値が未定義」であることを意味する「null」という予約語が定義されています。一方、JSONのように、データ自体にデータ構造の情報が含まれる（データとは別にスキーマが存在していない）、データにあっては、「フィールドが未定義」の場合を考慮する必要があります。

JSONデータに対して、N1QLによるクエリを実行した場合、他のドキュメントには存在するフィールドが、一部のドキュメントには存在しないことがあり得ます（RDB/SQLでは、テーブル内のすべてのレコードが同一のスキーマに従うため、このような状況が起こりません）。
N1QLには、そのようなギャップを埋めるため、@<tt>{MISSING}というキーワード（データ型）が用意されています。以下、N1QLにおけるデータ型としての、　@<tt>{NULL}と@<tt>{MISSING}の定義を確認した後に、N1QLにおける、存在しない値に対する比較演算について見ていきます。


==== @<tt>{MISSING}


N1QLにおいて@<tt>{MISSING}は、JSONドキュメントでフィールド（名前と値のペア）が存在していない（欠落している）ことを表します。
N1QLにおけるSELECT文で、検索結果として返されるフィールドが存在しない（MISSINGである）データがある場合、（クエリ処理において、内部的にはMISSINGはリテラル式で扱われますが）、最終的な結果の配列中では、（JSONデータにおける未定義の値である）NULLに変換されます。


==== @<tt>{NULL}


N1QLは、キーワードNULLを使用して空の値を表します。データ挿入、更新時に、フィールドの値をNULLにすることができます。


NULL値は、ゼロ除算や間違ったタイプの引数の受け渡しなど、特定の操作によっても生成され得ます。

NULLでは大文字と小文字が区別されません。たとえば、null、NULL、Null、およびnUllはすべて同等です。

Couchbase公式ドキュメント Data Types MISSING@<fn>{datatypes}

//footnote[datatypes][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/datatypes.html#datatypes]

=== 比較演算: @<tt>{IS [NOT] NULL|MISSING}


@<tt>{IS [NOT] NULL|MISSING}ファミリー演算子を使用すると、データ・セット内の属性の存在（または不在）に基づいて条件を指定できます。

これらは、データ型の定義から導かれる通りの挙動となります。

具体的なクエリと結果の例を見るのが、理解に役立つでしょう。


==== @<tt>{IS NULL}

//emlist[][sql]{
SELECT fname, children
    FROM tutorial.sample.person
       WHERE children IS NULL
//}

//emlist[][json]{
{
  "results": [
    {
      "children": null,
      "fname": "Fred"
    }
  ]
}
//}

==== @<tt>{IS MISSING}

//emlist[][sql]{
SELECT fname
   FROM tutorial.sample.person
      WHERE children IS MISSING
//}

//emlist[][json]{
{
  "results": [
    {
      "fname": "Harry"
    },
    {
      "fname": "Jane"
    }
  ]
}
//}

=== 比較演算: @<tt>{IS [NOT] VALUED}

NULLとMISSINGは、JSONデータ上明らかに異なる状態であり、区別される必要がある一方、アプリケーションにおける検索条件としては区別する必要がない場合が考えられます。その際にANDやORを使って複数の検索条件を並置する代わりに、N1QLでは、@<tt>{IS [NOT] VALUED}という比較演算が利用可能です。

 * @<tt>{IS VALUED}は、値が'MISSING'でも'NULL'でもない場合、TRUE（真）になります。
 * @<tt>{IS NOT VALUED}は、値が'MISSING'あるいは'NULL'である場合、TRUE（真）になります。


==== 参考情報

Couchbase公式ドキュメント Comparison Operators@<fn>{comparisonops}




=== SQLと比べた場合の制約

 * @<tt>{FULL [OUTER] JOIN} はサポートされていません
 * @<tt>{CROSS JOIN} はサポートされていません
 * @<tt>{RIGHT [OUTER] JOIN} は、JOIN連結の最初の (または唯一の)クエリである必要があります


====[column] エディションによる差異

クエリに関連した、コミュニティエディションには含まれない、エンタープライズエディションの機能として、以下があります。

 * ウィンドウ関数
 * FLEXインデックス（クエリからの全文検索インデックス利用）
 * コストベースオプティマイザ（CBO）
 * 無制限のクエリ同時実行
 * N1QLアグリゲートプッシュダウン
 * N1QLリクエストの監査
 * クエリのモニタリング

====[/column]

== 結合

Couchbase ServerのN1QLで利用することのできる、以下の3つの結合のタイプについて解説します。
 * @<tt>{ANSI JOIN}
 * @<tt>{Lookup JOIN}
 * @<tt>{Index JOIN}

下の二つのJOINでは、混同しやすいCouchbase Server独自のキーワードが用いられます。それらは、@<tt>{ON KEYS}と @<tt>{ON KEY ... FOR} です。

ここでは、これらの用法について、以下のデータモデルを用いて、例示による整理を行います。


==== データモデル仕様

 * 「ルート」と「エアライン(航空会社)」という、二つの種類のデータモデルがある
 * 「ルート」モデルのデータ構造は、「エアライン」への参照（複数）を含む。参照は、「エアライン」ドキュメントのキーと一致している。

=== ANSI JOIN

ANSI JOINでドキュメントキーを用いた結合を行うためには、@<tt>{META().id}を利用し、適切なインデックスが作成されている必要があります。 


==== シンタックス

//emlist{
lhs-expr
JOIN rhs-keyspace
ON any join condition
//}

==== クエリ例
//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline a
ON r.airlineid = META(a).id
//}

=== Lookup JOIN

@<tt>{ON KEYS} 句を用います。ドキュメントキーを用いた結合を行うために、インデックスを利用する必要はありません。 

==== シンタックス

//emlist{
lhs-expr
JOIN rhs-keyspace
ON KEYS lhs-expr.foreign-key
//}

==== クエリ例

//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline
ON KEYS r.airlineid
//}

=== Index JOIN

@<tt>{ON KEY ... FOR} 句を用います。下記のクエリ例では、Lookup JOINのケースと@<tt>{FROM ... JOIN ...}で指定されているキースペース（コレクション）の順序が異なっていることに注目ください。 
Index JOINでは、適切なインデックスが作成されている必要があります。

==== シンタックス

//emlist{
lhs-keyspace
JOIN rhs-keyspace
ON KEY rhs-keyspace.idx_key
FOR lhs-keyspace
//}

==== クエリ例

//emlist{
SELECT *
FROM `travel-sample`.inventory.airline a
JOIN `travel-sample`.inventory.route r
ON KEY r.airlineid
FOR a
//}


==== 参考情報

Couchbase公式ドキュメント JOIN Clause@<fn>{63c6596bad48de18597b283a703bfc2f}

== レンジスキャン

=== 分散データベース一般のレンジスキャン

分散データベースにおいては、一般的に（キーを用いた）レンジスキャンを効果的に行うためには、論理的に関連性のあるキーを設計するのみではなく、あらかじめそのレンジに含まれるデータが、同じ、あるいは近い場所（ノード、リージョン、パーティション）に存在する様に、（物理）設計を行う必要があります。

例えば、MongoDBでは、シャーディングの方法として、ハッシュとレンジのいずれかを要件に応じ選択することになります（この選択が物理配置に影響します）。HBaseでは、データはRowKeyで（物理的に）ソートされます。 

=== Couchbase Serverの特殊性

Couchbase Serverは、N1QLというSQLをJSONのために拡張したクエリ言語で、検索を行うことができます。その際、重要なことは、Couchbase Serverの中心的なアーキテクチャーはKVS(キーバリューストア)であり、そのレベルでは、バリューとしてJSONデータを格納しているのに過ぎないため、検索を行うための前提として、インデックスの定義・構築が行われている必要があります。

そもそも、スキーマレスなデータストアに対して、どうやって検索が可能なのか(全件サーチするのでなければ)？という視点からは、これはむしろ納得できるものとして受け止められるかと思います。

ただし、ここで留意が必要なのは、KVSのキーを使った検索（つまりキーを特定したアクセスではなく、ある範囲のキーを指定してデータを一度に取得するという要件）であっても、事前にインデックスの作成が欠かせない、というところです。
このことをよりよく理解するために、以下の二つの背景を押させておくことが重要です。

 * Couchbase　Serverにおいては、クエリおよびインデックスサービスは、（KVSである）データサービスとは、アーキテクチャー上完全に独立している（利用する要件がない場合は、無効にすることができ、そのために潜在的にリソースが消費されることはない）。

 * Dataサービスのキーのシャーディング方法には、ハッシュが用いられており、ユーザによって変更する余地を残していない。（CouchbaseServerは、メモリファーストアーキテクチャーと、クラスターマップを用いたクライアントからのノードへの直接のアクセスによって、データをチャンクで取り出すよりも、高い性能を実現している）

=== N1QLによるレンジスキャン

==== インデックスの作成

//emlist{
CREATE INDEX range_index ON MyFirstBucket(META().id);
//}

ここで、@<tt>{META().id} が、KVSのキー（ドキュメントID）に対応します。

@<tt>{MyFirstBucket} は、インデックスを作りたいバケットです。

インデックス作成のための構文は、@<tt>{CREATE INDEX [インデックス名] ON [キースペース名]([ドキュメントのフィールド名])}
というものです。バケット名の後のカッコ内に通常JSONドキュメントのフィールド名を指定しますが、ここではドキュメントの内容ではなく、ドキュメントが格納される際のキー/IDをインデックスとして利用するため @<tt>{META().id} という特殊なキーワードを用いています。

ここではキースペースの指定に@<tt>{MyFirstBucket} というバケット名のみを指定していますが、この場合、そのバケットの@<tt>{_default}スコープの@<tt>{_default}コレクションを指定したことになります。  

==== 利用例

下記の様な、キーにレンジを指定した検索が可能になります。

//emlist{
SELECT * from MyFirstBucket where Meta().id BETWEEN 'ABC:001' AND 'ABC:999';
//}

また、下記の様な、キーの一部を指定した曖昧検索も可能になります。

//emlist{
SELECT * from MyFirstBucket where Meta().id LIKE 'ABC:%';
//}

=== DataサービスAPIによるレンジスキャン

Couchbase Serverで「キー」のみによるレンジスキャンを行うケースとして、下記の様なDataサービスへのAPI呼び出しとして実現することが考えられます。
（ここでは、Javaを用いたコード例を示しています）

==== 同期型の例

//emlist{
protected List<Object> get() {
  for (String key : keys) {
    GetResult result = collection.get(String.valueOf(key));
    listResults.add(result);
  }
  return listResults;
}
//}

==== 非同期の例

//emlist{
protected List<Object>get() {
  ReactiveCollection reactiveCollection = collection.reactive();

  Flux<Object> resultFlux = Flux.fromArray(keys.toArray())
    .flatMap( k -> reactiveCollection.get(String.valueOf(k)));

  List<Object> listResults = resultFlux.collectList().block();
  return listResults;
}
//}

上記のコードは、コード外で、`id` のリストを作成し、`keys`リストとして与えられることを想定しています。

//footnote[63c6596bad48de18597b283a703bfc2f][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/join.html#index-join-clause]


//footnote[comparisonops][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/comparisonops.html]

== ユーザー定義関数

Couchbase ServerのN1QLでは、ユーザーが関数を定義して、組み込み関数と同じように、任意の式で呼び出すことができます。

====[column] エディションによる差異
ユーザー定義関数は、エンタープライズエディションでのみ利用することができます。

====[/column]

ユーザー定義関数には次の2つの種類があります。

 * @<strong>{インライン関数}は、サブクエリを含むN1QL式を使用して定義されます。クエリを簡素化するために、複雑な式や、何度も用いられる式に名前を付けて定義し、再利用することができます。
 * @<strong>{外部関数}は、N1QLではなく、プログラミング言語を使用して定義されます。この機能により、N1QL式を使用して定義するのが困難または不可能な関数を作成することができます。（構文上は、他の言語の利用も想定される書式が用いられていますが、現時点で）サポートされている言語はJavaScriptのみです。

ユーザー定義関数には次の2つの呼び出し方法があります。

 * @<strong>{N1QL}の任意の式中で、組み込み関数と同じように、呼び出すことができます。ただし、Couchbase Serverのユーザー定義関数は、単純なインライン関数を超えた高い自由度を持ち、関数の中でドキュメントの変更などを行うことができる反面、このような「副作用」を持つ、ユーザー定義関数をN1QL式の中から呼び出した場合には、エラーが発生することに注意が必要です。
 * @<strong>{EXECUTE FUNCTIONステートメント}を使って、ユーザー定義関数を単体で直接実行することが可能です。ユーザー定義関数をテストする際に用いることができる他、@<tt>{EXECUTE FUNCTION}ステートメントでは副作用のある関数を実行することが可能なため、RDBMSにおけるストアドプロシージャのような使い方ができます(なお、Couchbase Serverでは、RDBMSにおけるトリガに類似する機能としてEventingサービスを利用することができます)。

====[column] エディションによる差異
Eventingサービスは、エンタープライズエディションでのみ利用することができます。

====[/column]

==== 参考情報

Couchbase公式ドキュメント User-Defined Functions@<fn>{4fec4dc4c082b58a3a0c563887d0169d}

Couchbase公式ドキュメント CREATE FUNCTION@<fn>{9691b9034b80d65250e6f45a580041f0}

Couchbase公式ドキュメント EXECUTE FUNCTION@<fn>{973a71b323540fcd67ec61262da9d1de}


//footnote[4fec4dc4c082b58a3a0c563887d0169d][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/userfun.html]

//footnote[9691b9034b80d65250e6f45a580041f0][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/createfunction.html]

//footnote[973a71b323540fcd67ec61262da9d1de][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/execfunction.html]


