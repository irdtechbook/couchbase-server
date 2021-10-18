= インデックス

Couchbase Serverにおける、インデックス機能をRDBとの比較を交えながら説明します。

Couchbase Serverでは、RDBとは異なり、クエリの実行に際して、基本的に対応するインデックスの存在が前提となります。


== 概要

=== 用語法に関する注釈

Couchbase Serverにおけるインデックスは、グローバルセカンダリインデックス（GSI）と呼ばれています。別に「プライマリインデックス」、「セカンダリインデックス」という表現も用いられますが、これらはいずれも、グローバルセカンダリインデックス（GSI）の中のカテゴリーとなります（以下は、「プライマリインデックス」のメタデータの例です。@<tt>{"is_primary": true}かつ @<tt>{"using": "gsi"} であることがわかります）。

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

グローバルセカンダリインデックス（GSI）が登場する前には、データの検索のために、MapReduceビュー（View）という機能が使われていました（現在も互換性維持のために残されてはいますが、将来のリリースでは廃止されるとされています）。このViewが、データが存在するノードに対して「ローカル」であったことに対して、ローカリティに依存しない、クラスター単位で管理されるインデックスという意味で、「グローバル」という表現が用いられていると解釈することができます。このように、「グローバル」という語句は、別のローカルなインデックスとの対比で用いられているのではなく、Couchbase Serverにはローカルセカンダリインデックス(Local Secondary Index)という概念は存在しません。

また、「セカンダリ」という形容は、RDBにおけるセカンダリインデックスのように任意の数のインデックスを定義できる、という意味合いで受け取ることができます。

このように後続のテクノロジーであるという経緯から、Couchbase Serverの過去のバージョンではGSIとしてインデックスを作成する際に明示的に@<tt>{USING GSI}という句が用いられていました。
6.5以降のバージョンでは、 @<tt>{USING GSI}の利用はオプションであり、省略可能です。

=== RDBにおけるインデックスとの違い

Couchbase Serverのインデックスには、RDBにおけるインデックスと異なる、以下のような特徴があります。

//blankline

 * そのクエリを実行するために必要なインデックスが存在しない場合、処理が失敗します。
 * インデックスは、非同期で維持されます。
 * 同じ内容のインデックスを別名で複数定義できます。

//blankline

これらの特徴については、それぞれ関連する章で、背景やその特徴を踏まえた利用方法を説明していきます。

=== ストレージモデル

Couchbase Serverのグローバルセカンダリインデックス(GSI)には、以下のふたつのストレージモデルがあります。

//blankline

 * スタンダードGSI
 * メモリー最適化(Memory-optimized)GSI

====[column]エディションによる差異

メモリー最適化GSIは、コミュニティーエディションでは利用することができません。

====[/column]

スタンダードGSIは、ForestDBストレージエンジンを使用してBツリーインデックスを格納しています。また、最適なワーキングセットのデータをバッファに保持します。

メモリー最適化インデックスは、すべてのインデックスデータをメモリーに持ち、ロックフリースキップリストを使用してインデックスを保持しています。メモリー最適化インデックスは、インデックス更新時、より高速に処理が行われます。

スタンダードGSI、メモリー最適化GSIのいずれを使うかは、クラスター単位の設定となり、クラスターの初期構成時に、そのクラスターで使用するインデックスのタイプを選択することになります。

=== インデックス情報の確認

@<tt>{system:indexes}に対してクエリすることにより、インデックスに関する情報を確認することができます。

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

上記例のように、インデックスが存在するノード（@<tt>{datastore_id}）や状態（@<tt>{state}）など、インデックスに関する情報を確認することができます。



====[column]エディションによる差異

Indexサービスにおける、コミュニティーエディションには含まれない、エンタープライズエディションの機能として、以下があります。

//blankline

 * インデックスのパーティション化
 * インデックスレプリカ
 * メモリー最適化インデックスストレージ
 * Plasma（インデックス用の高速ストレージエンジン）
 * インデックスアドバイザー

====[/column]

== プライマリインデックス

プライマリインデックス@<fn>{indexing-and-query-perf-primary-index}は、キースペース単位で指定するインデックスです。

//footnote[indexing-and-query-perf-primary-index][https://docs.couchbase.com/server/current/learn/services-and-indexes/indexes/indexing-and-query-perf.html#primary-index]

====[column]ドキュメントの一意性について

Couchbase Serverでは、プライマリインデックスとは関係なく、ドキュメントキーはキースペースにおいて一意です。

====[/column]

=== 概要

クエリ実行時に、プライマリインデックスが使用されるケースとしては、以下があります。

//blankline

 * クエリにフィルター（述語・WHERE句）がない場合
 * クエリにフィルター（述語・WHERE句）があるが、該当するセカンダリインデックスがない場合

//blankline

Couchbase Serverでは、クエリを実行するために必要なインデックスが存在しない場合、処理が失敗します。
プライマリインデックスは、ドキュメントの特定のフィールドに限られない、キースペース（下記例の場合、@<tt>{airline}コレクション）全体に対して、そのような制約を回避するための最も簡便な方法になります。
プライマリインデックスが存在する場合、クエリの検索条件に使われている項目を個別に指定してインデックスを作成しなくても、クエリを実行することが可能になります。
ただし、結合を行う際など、プライマリインデックスで代用できない場合があります。


=== 定義方法

//emlist{
CREATE PRIMARY INDEX ON `travel-sample`.inventory.airline;
//}

作成されたインデックスの定義をWebコンソール等で確認するとわかりますが、上記のDDL(データ定義言語)は、下記のDDLを実行する際の糖衣構文(syntax sugar)にあたるものです。

//emlist{
CREATE PRIMARY INDEX `#primary` ON `travel-sample`.inventory.airline;
//}

Couchbase Serverのインデックスには名前を定義するのが通常であり、上掲の省略形はプライマリインデックスに特有のものです。
次の例に示すように、明示的に名前を付けて定義する構文が基本となります。

//emlist{
CREATE PRIMARY INDEX <インデックス名> ON <キースペース名>;
//}

====[column]インデックス設計のベストプラクティス

プライマリインデックスに頼った検索は検索条件の内容に関わらず、キースペース(上記例の場合、@<tt>{airline}コレクション)内のドキュメントに対する全件スキャンを実行することに留意が必要です。
これは、ドキュメントキーを検索条件(WHERE句)に用いたクエリの場合も同様です(ドキュメントキーを用いたセカンダリインデックスの定義については別に触れます)。

検索条件のないクエリ、つまりコレクションから全てのデータを取得するクエリを実行する場合には、プライマリインデックスを利用することになります。その場合も、条件を指定した個別の検索要件のためには、別にセカンダリインデックスを作成することによって、不要な全件スキャンが発生しないようにすることが重要です。

====[/column]

//blankline

====[column]アドホッククエリ要件について

ここで説明したインデックスの仕様について、柔軟性に欠けると思われる方もいるかもしれません。
Couchbase Serverのコア機能は、アプリケーションのバックエンドデータベースとして設計・最適化されています。このインデックスの仕様は、一般的なアプリケーションでは、ユーザーにより恣意的に記述されるクエリ(アドホッククエリ)は発生しない(必要とされない)ことを反映しているといえるでしょう。柔軟性は性能とのトレードオフになりえることを考えると、これは合理的な仕様だといえます。

一方、コア機能を補完する別のサービスが提供されています。たとえば、データ分析者によるヒストリカルデータへのアドホックなクエリという要件については、(エンタープライズエディションで提供される)Analyticsサービスが対応します。また、自由な語句の入力による全文検索という要件については、(本書では扱わない)Searchサービスが提供されています。

====[/column]

== セカンダリインデックス

=== 概要

JSONドキュメントの特定の要素（ドキュメントキー指定のためのメタデータ利用を含む）に対して設定するインデックスは、セカンダリインデックス@<fn>{indexing-and-query-perf-secondary-index}と呼ばれます。


//footnote[indexing-and-query-perf-secondary-index][https://docs.couchbase.com/server/current/learn/services-and-indexes/indexes/indexing-and-query-perf.html#secondary-index]

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

スカラー値のみではなく、JSONの他のデータ形式（配列、オブジェクト）を値として持つフィールドを用いることも可能です。この場合、述語条件の右側の値は同様に構造化されたデータ形式(配列、オブジェクト)である必要があります。


以下のように、ドット(@<tt>{.})表記（ノーテーション）により、ネストされたオブジェクトのフィールドを指定することもできます。

//emlist{
CREATE INDEX travel_geo_alt ON `travel-sample`.inventory.landmark(geo.alt);
CREATE INDEX travel_geo_lat ON `travel-sample`.inventory.landmark(geo.lat);
//}


ここで、@<tt>{geo} は、次のようなドキュメント内に埋め込まれたオブジェクトです。

//emlist{
{
 ...,
 "geo": {
  "alt": 12,
  "lat": 50.962097,
  "lon": 1.954764
 }
}
//}



=== ドキュメントキーの利用

セカンダリインデックスのキーとして、ドキュメントキーを用いることができます。ドキュメントキーは、ドキュメント内の情報ではないため、@<tt>{META()}キーワードを用います。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(META().id);
//}




== 配列インデックス

Couchbase Serverのインデックス定義DDLは、配列に対してインデックスを作成するための、特別な構文を備えています。


=== 要素オブジェクトの特定フィールド指定

JSONドキュメントは、スカラー、オブジェクト(サブドキュメント)、またはそれらの配列をフィールドとして持つことができます。

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

この@<tt>{schedule}配列に見られるような構造に対して、配列の要素であるオブジェクトの特定のフィールドにインデックスを付ける方法を次に示します。

//emlist{
CREATE INDEX idx_flight
ON `travel-sample`.inventory.route
( DISTINCT ARRAY v.flight FOR v IN schedule END );
//}

上のインデックス定義DDLは、以下の基本構文を元にしています。

//emlist{
ARRAY <インデックスキー> FOR v IN <配列名> END
//}

@<tt>{v} は、@<tt>{schedule}配列内の各要素オブジェクトを参照するために宣言された変数です。@<tt>{v.flight}は、配列の要素オブジェクトの@<tt>{flight}フィールドを示しています。


上記の例では、@<tt>{DISTINCT}キーワードにより、インデックスからキーの重複が除かれます。全てのキーを格納する場合は@<tt>{ALL}修飾子を利用します。

上記のインデックスにより、下記のように @<tt>{schedule.flight}に対して検索を行うことが可能になります。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE ANY v IN schedule SATISFIES v.flight LIKE 'UA%' END;
//}


=== 要素オブジェクトの複数フィールド指定

次に、@<tt>{schedule}(フライトスケジュール)の@<tt>{flight}(フライト番号)と@<tt>{day}(運行曜日)の情報を使って、インデックスを作成します。

複数のフィールドを指定する場合は、@<tt>{[...]} を使います。

//emlist{
CREATE INDEX idx_flight_and_day ON `travel-sample`.inventory.route
    (DISTINCT ARRAY [v.flight, v.day] FOR v IN schedule END);
//}

次のクエリは、週の２番目の曜日にスケジュールされたフライト番号「US681」のルートを検索します。

//emlist{
SELECT META().id FROM `travel-sample`.inventory.route
WHERE ANY v in schedule SATISFIES [v.flight, v.day] = ["US681", 2] END;
//}


=== 特定条件で選択された配列要素を利用

次に、週の最初の３日間に運行しているサンフランシスコからのフライトを指定して、インデックスを作成します。

下記のDDLでは、「@<tt>{ARRAY ... END}」構文の内部で、「 @<tt>{WHEN v.day < 4}」という検索条件が用いられているのがわかります。

//emlist{
CREATE INDEX idx_flight_early_week
ON `travel-sample`.inventory.route
( ALL ARRAY v.flight FOR v IN schedule WHEN v.day < 4 END )
WHERE sourceairport = "SFO";
//}

週の初日にスケジュールされた「UA」のフライトを検索するクエリを示します。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE sourceairport = "SFO" ①
AND ANY v IN schedule SATISFIES (v.flight LIKE 'UA%') ②
AND (v.day = 1) END; ③
//}

インデックス @<tt>{idx_flight_early_week} は、次のような関係において、上のクエリの対象となります。

//blankline

 ① 述語中の「@<tt>{sourceairport = "SFO"}」の箇所が、インデックスの @<tt>{WHERE}句に一致します。

 ② 述語中、@<tt>{ANY-SATISFIES}句の@<tt>{schedule}配列に関する検索のキーとして、@<tt>{v.flight} が一致します。

 ③ @<tt>{ANY-SATISFIES}句のもうひとつの条件「@<tt>{v.day = 1}」は、インデックス定義の@<tt>{WHEN}句の条件「 @<tt>{v.day < 4} 」に包含されます。



=== 要素オブジェクトの全てのフィールドを利用

次のDDLは、@<tt>{schedule}配列のすべての要素を含むインデックスを作成します。

//emlist{
CREATE INDEX idx_sched_simple
ON `travel-sample`.inventory.route(ALL schedule);
//}

次のクエリは、これまでよりも詳細な条件が指定された特定のスケジュールに一致するルートを検索します。

//emlist{
SELECT * FROM `travel-sample`.inventory.route
WHERE ANY v IN schedule
SATISFIES v = {"day":2, "flight": "US681", "utc": "19:20:00"} END; 
//}


== インデックス論理設計

インデックスの論理設計においては、要件に対して複数の選択肢がありえます。クエリの性能を最適化するために適切な選択を行うことが重要です。

=== 複合(Composit)インデックス

複数のキーを持つインデックスを定義することが可能です。

//emlist{
CREATE INDEX travel_info ON `travel-sample`.inventory.airline(name, id, icao, iata);
//}

複合(Composit)インデックスには、@<strong>{先行属性(leading attributes)マッチ}という性質があります。

上記インデックスは、@<tt>{name}と@<tt>{id}を用いた検索には利用可能ですが、@<tt>{name}を含まない、その他のフィールドを使った検索では、このインデックスは使用されません。
つまり、上記のインデックスでは４つのフィールドが指定されていますが、可能な組み合わせは、「@<tt>{name}のみ」、「@<tt>{name}と@<tt>{id}」、「@<tt>{name}と@<tt>{id}と@<tt>{icao}」、「@<tt>{name}と@<tt>{id}と@<tt>{icao}と@<tt>{iata}」の４通りになります。

複合インデックスの先行属性マッチという性質から、検索条件の組み合わせの数にあわせて、必要なインデックスの数が増えてしまう、という煩雑さが生じます。
そのような状況を回避するための機能として、適応(Adaptive)インデックスが用意されています。



=== カバリング(Covering)インデックス

クエリがインデックスのキーのみを参照している場合(つまり、検索条件としてだけでなく、SELECT句の中でもインデックスのキーのみが指定されている場合)、クエリエンジンは、データノードからフェッチすることなく、インデックススキャン結果を利用してクエリに応答することができます。

検索条件に用いる必要のないフィールドをあえてインデックスキーに含めて、複合(コンポジット)インデックスを作成することで、クエリの性能を向上することができる場合があります。

一方、インデックスの維持に必要なリソース負担が増加することに注意が必要です。


=== 部分(Partial)インデックス

インデックス定義DDLの中で、@<tt>{WHERE}句を用いることで、本来のデータに対して、部分的にインデックスを作成することができます。

下記の例では、@<tt>{activity} フィールドの値として @<tt>{eat}を持つドキュメントに対してのみインデックスが作成されます。

//emlist{
CREATE INDEX travel_eat ON `travel-sample`.inventory.landmark(name, id, address)
WHERE activity='eat';
//}

このインデックスがクエリに利用されるためには、そのクエリにも同じ条件が含まれている必要があります。

部分インデックスを使用する目的として、以下が考えられます。

//blankline

 * 必要な検索要件のみにインデックス化するデータの範囲を限定することによって、検索性能とリソース消費を最適化する。
 * (性別などのように)選択肢が限定的な(カーディナリティが低い)データを検索する場合に、選択肢毎に複数のインデックスに分割することによって、(たとえば、分散環境の効果と組み合わせて)性能を最適化する 。



=== 関数(Functional)インデックス

関数を適用したフィールドをキーとして使用し、インデックスを作成することができます。

インデックスには、関数適用後の値が格納されます。

複数ドキュメントに跨がる集計関数 (@<tt>{SUM}、@<tt>{AVG}、@<tt>{COUNT} 等)は使用できません。

//emlist{
CREATE INDEX travel_cxname ON `travel-sample`.inventory.airport(LOWER(name));
//}

上の例は@<tt>{name}フィールドの値として、大文字と小文字が混在しているケースを想定して、@<tt>{LOWER}関数を使って値を小文字に統一しています。

このインデックスがクエリに利用されるには、下のように、条件式の左辺、関数@<tt>{LOWER}と関数へのパラメータ@<tt>{name}の組み合わせがインデックスの定義と一致している必要があります。

//emlist{
SELECT * FROM `travel-sample`.inventory.airport
WHERE LOWER(name) = "villeneuve-sur-lot";
//}


より複雑な式の組み合わせを使用することもできます。たとえば以下のように、計算式を指定することも可能です。

//emlist{
CREATE INDEX travel_cx1 ON `travel-sample`.inventory.airport
(LOWER(name), ROUND(geo.alt * 0.3048));
//}

=== 適応(Adaptive)インデックス

@<tt>{DISTINCT PAIRS}構文を利用して、ドキュメントの複数のフィールドに対して、インデックスを付けることができます。
この場合、複合インデックスの場合と異なり、フィールドの指定順序にかかわらず、いずれかのフィールド(の組み合わせ)を検索条件としたクエリにインデックスを利用することが可能になります。
これにより、複数の検索条件から取捨選択の上、組み合わせて利用するような要件に対して、作成しなければならないインデックス数の増加を緩和することができます。

//emlist{
CREATE INDEX `ai_airport_day_faa`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS({airportname, city, faa}));
//}

さらに@<tt>{self}キーワードを用いることにより、ドキュメントの全てのフィールドを対象とすることができます。

//emlist{
CREATE INDEX `ai_self`
ON `travel-sample`.inventory.airport(DISTINCT PAIRS(self));
//}


== インデックス物理設計

インデックスの物理設計に関係するDDL構文を紹介し、性能最適化との関係について解説します。

=== 重複(Duplicate)インデックス

@<tt>{WITH}@<fn>{index-with}句を用いて、異なる名前で、共通した定義を持つインデックスを重複して定義することができます。

重複インデックスは、高可用性と負荷分散に役立ちます。

//emlist{
CREATE INDEX i1 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.1.0:8091"]};

CREATE INDEX i2 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.2.0:8091"]};

CREATE INDEX i3 ON `travel-sample`.inventory.airport(LOWER(name), id, icao)
WHERE country = 'France' WITH {"nodes": ["192.0.3.0:8091"]};
//}

上の例では、3つのインデックスはすべて、同じキーと同じWHERE句を持っています。
異なっている部分としては、インデックスの名前の他、@<tt>{WITH}句で、それぞれ別のノードを指定しています。

@<tt>{WITH}句でノードを指定しない場合であっても、重複した内容を持つインデックスの作成は許されており、その場合、物理配置はランダムに決定されます。

クエリ実行時、重複インデックスはラウンドロビン方式で使用され、負荷が分散されます。


//footnote[index-with][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/createindex.html#index-with]


====[column]エディションによる差異

エンタープライズエディションでは、ひとつのインデックス定義DDLを使って、下記のように複数のノード指定を行うことができます。

//blankline

@<tt>{WITH {"nodes":["node1:8091", "node2:8091", "node3:8091"]\}}


====[/column]

//blankline

====[column]WITH句のその他の利用方法 

本書では詳細は触れませんが、@<tt>{WITH}句で指定できる項目には、@<tt>{node}の他に、@<tt>{num_replica}(インデックスレプリカの数)や(真偽値をとる)@<tt>{defer_build}(インデックスの遅延作成)があります。

====[/column]

=== パーティション

@<tt>{PARTITION BY}句を用いて、インデックスを複数のパーティションに分割することができます。

インデックスのパーティション化@<fn>{index-partitioning}には、次のような利点があります。

//blankline

 * @<strong>{スケールアウト} インデックスサイズの増大に対する、水平方向の拡張による性能最適化が可能です。
 * @<strong>{並行スキャン} 複数のパーティションを同時に並行してスキャン可能なため、クエリ待ち時間の短縮効果が期待できます。
 * @<strong>{透過性} パーティション分割の状態によって、クエリを変更する必要はありません。

//blankline

ここでは、限られた例を示すに留めますが、たとえば、次のインデックス定義DDLはドキュメントキーでパーティション化されたインデックスを作成します。

//emlist{
CREATE INDEX idx ON `travel-sample`.inventory.airline
(country, name, id)
PARTITION BY HASH(META().id);
//}

====[column]エディションによる差異

インデックスのパーティション化は、エンタープライズエディションでのみ利用できます。

====[/column]


//footnote[index-partitioning][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/index-partitioning.html]

== 先行属性マッチと性能最適化

=== カーディナリティ

一般にデータベースにおけるインデックススキャンについては、カーディナリティが高い(データの種類が多い)項目の検索に向いているということがいえます。これは、カーディナリティが低い(データの種類が少ない)項目の検索では、実際上、フルスキャンに違い動きになる(フルスキャンが向いている)のと裏返しの関係にあります。

Couchbase Serverのインデックスにおいても、属性リストの先頭に高いカーディナリティの属性を配置することで、インデックスの検索性能を最適化することができます。
ただし、ここで重要なのは、先行属性(leading attributes)の一致を妨げない限り、という条件が付くことです。

クエリに一致するインデックスの決定は、以下の順序で行われます。

//blankline

① キースペースマッチ

② 条件句マッチ

③ 先行属性マッチ

//blankline

具体的には、クエリ中の下記の要素が対応します。

//emlist{
SELECT *
FROM retail.east.customer c ①
WHERE c.age = $age ③
AND c.grade = "premium"; ②
//}

//emlist{
CREATE INDEX idx_cust ON retail.east.customer(age, sex)
WHERE grade ="premium";
//}

上記のクエリとインデックスの例は、先行属性マッチとカーディナリティの関係が適切な例となります。

ただし、下記のようなクエリでは、上のインデックスを用いることができません。

//emlist{
SELECT *
FROM retail.east.customer c 
WHERE c.sex = $sex
AND c.grade = "premium";
//}

=== DataサービスAPIとの併用

クエリによる検索の結果として、多数の(かつ大きな)ドキュメントが返されるような場合、クエリではドキュメントキーをクライアントに返し、クライアントからDataサービスAPIを使用してドキュメントをフェッチすることが考えられます。
これにより、Queryサービスのプロセスのメモリー領域がボトルネックとなることを避けることができます。さらに、Couchbase Serverでは、ドキュメントキーによるデータ取得はそのドキュメントを管理しているノードから直接行われるため、ひとつの大きなネットワーク伝送を、多数の小さなネットワーク伝送に置き換えることが可能になります。

クエリのサンプルを示します。

//emlist{
SELECT META().id as docKey
FROM retail.east.order
WHERE META().id IS NOT MISSING
AND status = "Pending";
//}


下記は、上記クエリ実行のためのインデックス定義です。

//emlist{
CREATE INDEX idx_order_pending ON retail.east.order(META().id)
WHERE status = "Pending";
//}


上記クエリでは、@<tt>{META().id IS NOT MISSING}という条件が用いられています。
@<tt>{META().id}を持たないドキュメントは存在しないため、これは一見不要にも思われますが、@<tt>{META().id}に対する先行属性マッチのために必要になります。

//blankline

DataサービスAPIの利用方法については、アプリケーション開発を主題とした最終章の「キーバリュー操作」に関する解説を参照してください。