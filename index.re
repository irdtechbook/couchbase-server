= インデックス


Couchbase Serverでは、RDBとは異なり、クエリの実行にあたり対応するインデックスが存在していることが前提となります。
Couchbase Serverにおける、インデックス機能の基本をRDBとの比較を交えながら説明します。

== インデックス概要

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

グローバルセカンダリーインデックス（GSI）が登場する前には、データの検索のために、MapReduceビュー（View）という機能が使われていました（現在も互換性維持のために残されてはいますが、将来のリリースでは廃止されるとされています）。このViewがデータノードに対して「ローカル」であったことに対して、データノードのローカリティに依存しない、クラスター単位で管理されるインデックスという意味で、「グローバル」という表現が用いられていると解釈することができます。「セカンダリー」という形容には、RDBにおけるセカンダリーインデックスのように任意の数のインデックスを定義できる、というニュアンスで受け止めることができます。このように後続のテクノロジーであるという経緯から、Couchbase Serverの過去のバージョンではGSIとしてインデックスを作成する際に明示的に@<tt>{USING GSI}という句が用いられていました。6.5以降のバージョンでは、 @<tt>{USING GSI}の利用はオプションであり、省略可能です。なお、Couchbase Serverには、ローカルセカンダリーインデックス(Local Secondary Index)という概念は存在しません。


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

====[column]エディションによる差異
メモリ最適化GSIは、コミュニティエディションでは利用することができません。

====[/column]

スタンダードGSIは、ForestDBストレージエンジンを使用してBツリーインデックスを格納しています。また、最適なワーキングセットのデータをバッファに保持します。

メモリ最適化インデックスは、すべてのインデックスデータをメモリに持ち、ロックフリースキップリストを使用してインデックスを保持しています。メモリ最適化インデックスは、インデックス更新時、より高速に処理が行われます。

スタンダードGSI、メモリ最適化GSIのいずれを使うかは、クラスター単位の設定となり、クラスターの初期構成時に、そのクラスターで使用するインデックスのタイプを選択することになります。



== プライマリインデックス

プライマリインデックスは、キースペース単位で指定するインデックスです。

====[column]ドキュメントの一意性について
Couchbase Serverでは、プライマリインデックスとは関係なく、ドキュメントキーはキーペースにおいて一意です。

====[/column]

=== 概要

クエリ実行時に、プライマリインデックスが使用されるケースとしては、以下があります。

 * クエリにフィルタ（述語・WHERE句）がない場合
 * クエリにフィルタ（述語・WHERE句）があるが、該当するセカンダリーグローバルインデックスがない場合

Couchbase Serverでは、クエリを実行するために必要なインデックスが存在しない場合、処理が失敗します。
プライマリインデックスは、ドキュメントの特定のフィールドに限られない、キースペース（下記例の場合指定された@<tt>{airline}コレクション）全体に対して、インデックスを作成することができるため、そのような制約を回避するための最も簡便な方法になります。
つまり、クエリの検索条件に使われている項目を個別に指定してインデックスを作成しなくても、クエリを実行することが可能になります。ただし、結合を行う際など、プライマリインデックスで代用できない場合があります。

=== 定義方法

//emlist{
CREATE PRIMARY INDEX ON `travel-sample`.inventory.airline;
//}

作成されたインデックスの定義（ @<tt>{Definition}）を確認すると分かりますが、上記のDDLは、下記のDDLを実行する際の糖衣構文(syntax sugar)にあたるものです。(Couchbase Serverのインデックスには名前を定義するのが通常であり、上記の省略形は、プライマリインデックスに特有のものです)

//emlist{
CREATE PRIMARY INDEX `#primary` ON `travel-sample`.inventory.airline
//}

したがって当然、次の例に示すように、明示的に、名前を付けて定義することもできます。

//emlist{
CREATE PRIMARY INDEX <インデックス名> ON <キースペース名>;
//}

====[column]実運用におけるベストプラクティス
プライマリインデックスによる検索は、検索条件の内容に関わらず、キースペース(上記例の場合指定された@<tt>{airline}コレクション)内のドキュメントに対する全件スキャンを実行することに留意が必要です。
これは、ドキュメントキーによる検索の場合も同様です(ドキュメントキーを用いたセカンダリーインデックスの定義については別に触れます)。
実際上、プライマリインデックスは、開発時の利便性のためのものであると捉えることができます。実運用においては、プライマリインデックスを用いないことがベストプラクティスとなります。

====[/column]

====[column]アダプティブ(Adaptive)インデックス
アドホックなクエリのために、本書の別の箇所で紹介しているアダプティブ（Adaptive）インデックスの利用を検討することができます。
アダプティブインデックスの利用により、プライマリーインデックスの特徴である全件スキャンと、それに伴う検索性能劣化を避けることができます。ただし、アダプティブインデックスによってインデックス定義の自由度は増す事になりますが、それでも検索対象フィールドを特定する必要はあり、インデックス作成時に考慮されていなかったフィールドを用いて検索することもできる、プライマリインデックスの同等物と見なすことができる訳ではありません。

====[/column]

== セカンダリインデックス

=== 概要

JSONドキュメントの特定の要素（フィールドまたはドキュメントのキー）に対して設定するインデックスは、セカンダリインデックスと呼ばれます（プライマリインデックスは要素を指定せず、キースペース単位で定義されます）。


=== 定義方法

まずは以下、JSONのフィールドの扱いの違いによる定義方法を見ていきます。

単純にフィールド名を指定した場合、ドキュメントのトップレベルのフィールドを指します。

//emlist{
CREATE INDEX travel_name ON `travel-sample`.inventory.airline(name);
//}

このキースペースにおいて、@<tt>{name} は次のような単純なスカラー値です。

//emlist{
{ "name": "Air France" }
//}

述語条件の左項は、スカラー値のみではなく、JSONの他のデータ形式（配列、オブジェクト）を用いることも可能ですが、この場合、述語条件の右側の値は同様に構造化されたデータ形式（配列、オブジェクト）である必要があります。


ドット(@<tt>{.})表記（ノーテーション）により、ネストされたオブジェクト（のフィールド）を指定することもできます。

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



=== ドキュメントキー

セカンダリインデックスのキーとして、ドキュメントキー(ドキュメントID)を用いることができます。ドキュメントキーは、ドキュメント内の情報ではないため、　@<tt>{META()}　キーワードを用います。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(META().id);
//}



=== メタデータ

@<tt>{system:indexes}に対してクエリすることにより、インデックスのメタデータを確認することができます。

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



====[column]エディションによる差異

Indexサービスにおける、コミュニティエディションには含まれない、エンタープライズエディションの機能として、以下があります。

 * インデックスのパーティション化
 * インデックスレプリカ
 * メモリ最適化インデックスストレージ
 * Plasma（インデックス用の高速ストレージエンジン）
 * インデックスアドバイザー

コミュニティエディションユーザーは、右のサイトを使って、インデックスアドバイザー機能を利用することができます：https://index-advisor.couchbase.com/indexadvisor 
コミュニティエディションでは、WebコンソールのQuery Workbench画面に、「External Query Advisor」という上記サイトへのリンクが表示されます。

====[/column]

=== 参考情報

Couchbase公式ドキュメント Using Indexes@<fn>{global-secondary-indexes}

//footnote[global-secondary-indexes][https://docs.couchbase.com/server/current/learn/services-and-indexes/indexes/global-secondary-indexes.html]


== 配列インデックス

Couchbase Serverのインデックス定義DDLは、配列に対してインデックスを作成するための、特別な構文を備えています。


=== 配列インデックスの基本

JSONは、スカラー、オブジェクト、または配列をフィールドとして持つことができます。
各オブジェクトは、他のオブジェクトや配列をネストできます。各配列は他のオブジェクトや配列を持つことができます。

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

@<tt>{schedule}配列に見られるような複雑な構造に対して、サブオブジェクト内の特定の配列またはフィールドにインデックスを付ける方法を次に示します。

//emlist{
CREATE INDEX idx_flight
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

@<tt>{ARRAY <インデックスキー> FOR v IN <配列名> END}構文と合わせて利用することのできるキーワードを使って、より複雑なインデックス作成を行うことができます。

=== 配列要素中の複数フィールドによるインデックス作成

@<tt>{schedule}(フライトスケジュール)の@<tt>{flight}(フライト番号)と@<tt>{day}(運行曜日)の情報を使ってインデックス @<tt>{idx_flight_and_day} を作成します。
複数のフィールド(@<tt>{flight}と@<tt>{day})を指定する場合は、@<tt>{[...]} を使います。

//emlist{
CREATE INDEX idx_flight_and_day ON `travel-sample`.inventory.route
    (DISTINCT ARRAY [v.flight, v.day] FOR v IN schedule END);
//}

2日にスケジュールされた「US681」フライトのリストを検索します。

//emlist{
SELECT meta().id FROM `travel-sample`.inventory.route
WHERE ANY v in schedule SATISFIES [v.flight, v.day] = ["US681", 2] END;
//}


=== 特定条件で選択された配列要素によるインデックス作成

週の最初の３日間に運行しているサンフランシスコからのフライトのインデックス@<tt>{idx_flight_early_week}を作成します。

下記のステートメント中、@<tt>{ARRAY <インデックスキー> FOR v IN <配列名> END}構文の内部で、@<tt>{WHEN v.day < 4}という検索条件が用いられているのが分かります。

//emlist{
CREATE INDEX idx_flight_early_week
ON `travel-sample`.inventory.route
( ALL ARRAY v.flight FOR v IN schedule WHEN v.day < 4 END )
WHERE sourceairport = "SFO";
//}

上記のインデックスを使う、週の初日にスケジュールされた「UA」フライトのリストを検索するクエリを示します。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE sourceairport = "SFO" ①
AND ANY v IN schedule SATISFIES (v.flight LIKE 'UA%') ②
AND (v.day=1) END; ③
//}

インデックス @<tt>{idx_flight_early_week} は、次のような関係において、上のクエリの対象となります。

① クエリ述語中の @<tt>{sourceairport = "SFO"} の部分は、インデックスの @<tt>{WHERE}句に一致します。

② クエリ述語中 @<tt>{ANY-SATISFIES} 句の@<tt>{schedule}配列に関する検索のキーとして、インデックスキー @<tt>{v.flight} が一致します。

③ クエリ述語中 @<tt>{ANY-SATISFIES} 句のもう一つの条件 @<tt>{v.day=1}は、インデックス定義の @<tt>{WHEN} 句の条件「 @<tt>{v.day < 4} 」に包含されます。



=== 簡略化構文によるインデックス作成

次のステートメントは、@<tt>{schedule}配列のすべての要素を含むインデックスを作成します。

//emlist{
CREATE INDEX idx_sched_simple
ON `travel-sample`.inventory.route (ALL schedule);
//}

次のクエリは、特定のスケジュールに一致するルートを検索します

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE ANY v IN schedule
SATISFIES v = {"day":2, "flight": "US681", "utc": "19:20:00"} END; 
//}

上記のインデックスにおいては、配列の要素から特定のフィールドが選択されておらず、@<tt>{schedule}配列の要素はオブジェクトであるため、このインデックスを利用するためには、検索条件の右辺の値は同様にオブジェクトである必要があります。

N1QLの@<tt>{UNNEST}キーワードを使って、上のクエリを下記のように書き換えることもできます。

//emlist{
SELECT * FROM `travel-sample`.inventory.route t
UNNEST schedule sch
WHERE sch = {"day":2, "flight": "US681", "utc": "19:20:00"};
//}

== インデックス論理設計

インデックスの論理設計においては、要件に対して複数の選択肢があり得ます、クエリの性能を最適化するために適切な選択を行うことが重要です。

=== 複合(コンポジット)インデックス

複数のキーを持つインデックスを定義することが可能です。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(name, id, icao, iata);
//}

=== カバリングインデックス

クエリがインデックス内のキーのみを参照している場合、クエリエンジンは、データノードからフェッチすることなく、インデックススキャン結果のみを利用してクエリに応答することができます。

複合(コンポジット)インデックスとして、検索に用いる必要のないキーをあえてインデックスに含めることで、特定のクエリの性能を向上することができる場合があります。
一方、インデックスの維持に必要なリソース負担が増加することに注意が必要です。


=== パーシャル(部分)インデックス

@<tt>{CREATE INDEX} DDLの定義に @<tt>{WHERE}句を用いることで、本来のデータに対して、部分的にインデックスを作成することができます。

//emlist{
CREATE INDEX travel_eat ON `travel-sample`.inventory.landmark(name, id, address)
WHERE activity='eat';
//}

上記例では、@<tt>{activity} フィールドの値として @<tt>{eat}を持つ、ドキュメントに対してのみインデックスが作成されます。
このインデックスがクエリに利用されるためは、そのクエリにも同じ条件が含まれている必要があります。

パーシャルインデックスを使用する目的として以下が考えられます。

 * 必要な検索要件のみに特化したインデックス化範囲の限定による性能の最適化
 * 検索の選択肢が限定的な場合(性別や地域名など)に、複数のインデックスに分割することによる性能の最適化(この場合、インデックス定義時のノード配置の指定と組み合わせて、分散環境の効果を活用することが考えられます）



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

=== アダプティブ(Adaptive)インデックス

@<tt>{DISTINCT PAIRS}構文を利用して、ドキュメントの指定されたフィールドに対して、インデックスを付けることができます。
この場合、単純な複合インデックスと異なり、フィールドの順序にかかわらず、全ての組み合わせを用いてクエリを実行することが可能になります。

これにより、クエリのさまざまな組み合わせに対して、それぞれ対応する複合インデックスを作成する必要がなく、効率的にアドホックなクエリの要件を満たすことができます。

アダプティブインデックス、配列インデックスの特殊なタイプとして位置付けられます。

//emlist{
CREATE INDEX `ai_airport_day_faa`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS({airportname, city, faa}));
//}

さらに`self`キーワードを用いることにより、全てのフィールドを対象とすることができます。

//emlist{
CREATE INDEX `ai_self`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS(self));
//}


== インデックス物理設計

インデックスの物理設計に関係するDDL構文を紹介し、性能最適化との関係について解説します。

=== 重複(Duplicate)インデックス

@<tt>{WITH}句を用いて、異なる名前で、共通した定義を持つインデックスを重複して定義することができます。

重複インデックスは、高可用性と負荷分散に役立ちます。

//emlist{
CREATE INDEX i1 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.1.0:8091"]};

CREATE INDEX i2 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.2.0:8091"]};

CREATE INDEX i3 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.3.0:8091"]};
//}

上の例では、3つのインデックスはすべて、同じキーと同じ@<tt>{WHERE}句を持っています。唯一の違いは、インデックスの名前です。
@<tt>{CREATE INDEX} ステートメントの @<tt>{WITH}句を使用して、物理的な場所を指定します。
場所を指定しない場合であっても、重複した内容を持つインデックスの作成は許されており、その場合物理配置はランダムに決定されます。

クエリ実行時、最適化の一貫として、重複インデックスはラウンドロビン方式で使用され、負荷が分散されます。

====[column]エディションによる差異

エンタープライズエディションでは、1つのインデックス作成時に下記のように複数のノード指定を行うことができます。

//emlist{
CREATE INDEX iDuplicate ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WITH {"nodes":["node1:8091", "node2:8091", "node3:8091"]};
//}

====[/column]

====[column]WITH句のその他の利用方法 
本書では詳細は触れませんが、@<tt>{WITH}句には、@<tt>{node}の他に、@<tt>{num_replica}(インデックスレプリカの数)、そして真偽値をとる@<tt>{defer_build}(インデックスの遅延作成)があります。

====[/column]

=== パーティション

@<tt>{PARTITION BY}句を用いて、インデックスを複数のパーティションに分割することができます。

インデックスのパーティション化には、次のような利点があります。

 * @<strong>{スケールアウト} インデックスサイズの増大に対する、水平方向の拡張による性能最適化が可能です。
 * @<strong>{並行スキャン} 複数のパーティションを同時に並行してスキャン可能なため、クエリ待ち時間の短縮効果が期待できます。
 * @<strong>{クエリに対する透明性} パーティションの状態によって、クエリを変更する必要はありません。

ここでは、限られた例を示すに留めますが、例えば、次のステートメントはドキュメントIDでパーティション化されたインデックスを作成します。

//emlist{
CREATE INDEX idx ON `travel-sample`.inventory.airline
(country, name, id)
 PARTITION BY HASH(META().id);
//}

====[column]エディションによる差異
インデックスパーティショニングは、エンタープライズエディションでのみ利用できます。

====[/column]

== 性能最適化のための観点

=== カーディナリティ

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

=== 多数のドキュメントの取得

クエリによりある条件を用いた検索により、多数の(かつ大きな)ドキュメントが返されるような場合、クエリではドキュメントキーをクライアントに返し、クライアントからDataサービスAPIを使用してドキュメントをフェッチすることが考えられます。
これにより、Queryサービスのプロセスのメモリ領域がボトルネックとなる事を避けることができます。さらに、Couchbase Serverでは、ドキュメントキーによるデータ取得はそのドキュメントを管理しているノードから直接行われるため、1つの大きなネットワーク伝送を、多数の小さなネットワーク伝送に置き換えることが可能になります。

下記にクエリのサンプルを示します。

//emlist{
SELECT META().id as docKey
FROM retail.east.order
WHERE META().id IS NOT MISSING
AND status = "Pending"
//}


上記クエリでは、@<tt>{META().id IS NOT MISSING}という条件が用いられています。
@<tt>{META().id}を持たないドキュメントは存在しないため一見不要にも思われますが、クエリが先行属性マッチにより一致するインデックスを利用するために必要になります。

下記は、上記クエリ実行のためのインデックス定義です。

//emlist{
CREATE INDEX idx_order_pending ON retail.east.order(META().id)
WHERE status = "Pending";
//}

