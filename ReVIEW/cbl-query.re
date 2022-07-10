
= Couchbase Liteクエリ

== 概要

Couchbase Liteでは、JSONデータに対してSQL形式のクエリを行うことができます。

=== クエリ形式

Couchbase Liteは、標準SQL同様の以下の形式のクエリを使用します。


//emlist[][sql]{
SELECT ____ 
FROM ____ 
WHERE ____ 
JOIN ____ 
GROUP BY ____ 
ORDER BY ____
//}


SQL経験者にとっては馴染みのあるものだと思いますが、各行について説明すると以下の通りです。

//blankline

 * @<tt>{SELECT}ステートメントには、クエリの結果セットを指定します。
 * @<tt>{FROM}には、クエリの照会先を指定します。
 * @<tt>{WHERE}ステートメントでは、検索条件を指定します。
 * @<tt>{JOIN}ステートメントには、複数の照会先を結合するための条件を指定します。
 * @<tt>{GROUP BY}ステートメントには、結果セットとして返されるアイテムをグループ化するために使用される基準を指定します。
 * @<tt>{ORDER BY}ステートメントには、結果セット内のアイテムの順序付けに使用される基準を指定します。

//blankline

このように、クエリ形式の構造とセマンティクスは、SQLに基づきます。
さらに、SQLには含まれていないJSONデータの操作に関する関する部分は、N1QLクエリ言語に基づいています。

=== N1QL

N1QLという名前は、リレーショナルデータモデルにおける第一正規型(first normal form)と関係し、非第一正規型クエリ言語(Non 1st normal form Query Language)の略称です。
N1QLと書いて、ニッケルと読みます。
なお、N1QLという名称はCouchbase固有のものです。@<fn>{n1ql-note}

N1QLの設計は、SQL++に由来します。

//footnote[n1ql-note][なお、同じN1QLという名称であっても、Couchbase LiteとCouchbase Serverとでは異なる部分があります。詳細については、ドキュメントを参照ください。]

=== SQL++

SQL++はカリフォルニア大学で生み出され、Apache AsterixDB@<fn>{apache-asterixdb}でも採用されています。

SQL++の提唱者Yannis Papakonstantinou他の著者による論文「The SQL++ Query Language: Configurable, Unifying and Semi-structured@<fn>{arxiv-sqlplusplus}」では、N1QLについて「syntactic sugar over SQL++」という表現が用いられています。

//footnote[apache-asterixdb][https://asterixdb.apache.org/]
//footnote[arxiv-sqlplusplus][https://arxiv.org/abs/1405.3631]

=== クエリビルダーAPIとSQL++/N1QLクエリAPI

Couchbase Liteクエリには、以下の二つタイプのAPIが存在します。

//blankline

 * @<em>{クエリビルダーAPI}: クエリを文字列で表現するのではなく、オブジェクトとして構築します。クエリの構築には、ビルダークラス(@<tt>{QueryBuilder})を使用します。
 * @<em>{SQL++/N1QLクエリAPI}@<fn>{n1ql-sql-note}: クエリ文字列を使います。

//blankline

//footnote[n1ql-sql-note][本書の以降の記述では「SQL++/N1QL」という並列表記を用います。Couchbase Mobile 3.0以降のドキュメントでは、Couchbaseのクエリ言語を指す名称として「SQL++」という名称が用いられるようになっている一方、既存の情報では「N1QL」という名称が用いられています。読者がそれらの情報を参照する際の混乱を避けるために、「SQL++/N1QL」という並列表記を採用しています。]

クエリビルダーAPIでオブジェクトとしてクエリを構築する場合も、その構文構造は、上述したSQLの形式と一致しています。

//blankline

二つのAPIは、プログラミング方法は異なるものの、機能レベルでは基本的に同じ機能を提供しますが、僅かながら機能における差分が存在します。

クエリビルダーAPIは、SQL++/N1QLクエリAPIが備えている機能の全てを備えている訳ではありません。
一方で、Couchbase Lite 2.xまでは、クエリAPIは存在しておらず、クエリビルダーが唯一のAPIでした。
ここからも分かる通り、この機能の差は利用において本質的な問題とはなり得ないものであり、クエリビルダーAPIに備わっていない機能を利用するために、SQL++/N1QLクエリAPIと併用する必要性は基本的に生じません。
例えば、データ型変換関数はクエリAPIでのみ提供されていますが、クエリビルダーAPIを使う場合は、プログラミング言語の機能を利用することができます。

反対に、SQL++/N1QLクエリAPIはクエリビルダーAPIには備わっていない機能を備えている一方、APIに依存しなければならない操作があります。
このような例に、インデックス作成があります。
Couchbase Liteにおけるインデックス作成は、DDLクエリではなくAPIを用います。


//footnote[predictive-query-note][予測クエリは、エンタープライズエディションの機能です。また、全てのプログラミング言語でこの機能が提供されるわけではありません。]



====[column]C APIの制約とその背景

Couchbase Lite C API@<fn>{github-couchbase-lite-C}では、クエリビルダーAPIは提供されておらず、SQL++/N1QLクエリAPIのみが提供されます。

C言語におけるこの制約の理由として、クエリの構築をクエリビルダーAPIのような方法で実現するには、(純粋な)Cの構文では表現力が不足している、と考えることができます。
なお、C APIには、@<tt>{MutableDocument}クラスのように、C++の文法に由来するAPIが含まれていますが、これらは「非安定(volatile)」、「最終化されておらず、将来のリリースで変更する可能性がある(not finalized, and may change in future releases)」とされています。

クエリ文字列の利用には、コンパイル時に検証されず、タイプセーフではない等の欠点がありますが、Flutterのようなサポートされていないプログラミング言語のために、Couchbase Lite用のパッケージをC言語で構築する際には、クラスを介して実現するよりも、クエリ文字列を用いる方が柔軟で実装が容易である、という利点があります。
この点も、C SDKではSQL++/N1QLクエリAPIのみが提供されていることを理解する助けになるでしょう。

====[/column]


//footnote[github-couchbase-lite-C][https://github.com/couchbase/couchbase-lite-C]

=== 標準SQLからの主な拡張要素

Couchbase LiteはJSONを扱うドキュメントデータベースであるため、クエリの構文はSQLに基づきながら、JSONデータの扱いのために拡張されています。
以下に、主要な拡張要素を示します。

//blankline

 * @<em>{未定義データ型}: テーブルスキーマを有するデータベースへのクエリと異なり、N1QLでは、未定義のデータを表現するためのデータ型@<tt>{MISSING}が存在します。また、@<tt>{MISSING}値に対する検索条件を指定する方法があります。
 * @<em>{辞書型オブジェクト}: データ型として辞書型オブジェクトの表現、およびそれを想定した検索条件指定方法があります。
 * @<em>{配列}: データ型として配列の表現、およびそれを想定した検索条件指定方法があります。

== 定義と実行

クエリビルダーAPIとSQL++/N1QLクエリAPIの詳細については、後の章でそれぞれ解説します。
ここでは、基本的な定義と両方に共通する実行方法を説明します。

=== クエリビルダーAPI

@<tt>{QueryBuilder}クラスのメソッドを使用して、@<tt>{Query}オブジェクトを作成します。

以下は、その例です。

//emlist[][Java]{
Query query = QueryBuilder
    .select(SelectResult.all())
    .from(DataSource.database(database))
    .where(Expression.property("type").equalTo(Expression.string("hotel")))
    .limit(Expression.intValue(10));
//}

@<tt>{Query}オブジェクトの@<tt>{execute}メソッドを使用してクエリを実行します。

//emlist[][Java]{
ResultSet rs = query.execute();
//}

以下は、@<tt>{ResultSet}オブジェクトから、@<tt>{Result}オブジェクトを順番に処理する例です。

//emlist[][Java]{
for (Result result : rs) {
    // 何らかの処理を行う
}
//}

結果として返されるデータの構造は、クエリの記載方法によって異なります。これについては、後の章で解説します。


=== SQL++/N1QLクエリAPI


@<tt>{Database}クラスの@<tt>{createQuery}メソッドを使用して、クエリ文字列から@<tt>{Query}オブジェクトを作成します。


以下は、クエリ文字列から、@<tt>{Query}オブジェクトを作成する例です。


//emlist[][Java]{
Query query = database.createQuery("SELECT * FROM database"); 
//}

@<tt>{Query}オブジェクトの実行以降の操作については、クエリービルダーAPIと同様です。


== ライブクエリ

=== 概要

@<tt>{Query}オブジェクトに対してリスナーを設定することで、クエリの結果に影響を与える変更に対する通知を受けることができます。
このようなクエリは、ライブクエリ(Live Query@<fn>{docs-java-query-live})と呼ばれます。

ライブクエリは、データを最新の状態に保つ必要のあるテーブルやリストビューなどのユーザーインターフェイスを構築するための方法を提供します。

//footnote[docs-java-query-live][https://docs.couchbase.com/couchbase-lite/3.0/java/query-live.html]


=== 利用方法

以下、コーディング例を示します。

//emlist[][java]{
// Queryオブジェクトの作成
Query query = QueryBuilder.select(SelectResult.all())
    .from(DataSource.database(database));

// リスナー追加によるライブクエリのアクティブ化
ListenerToken token = query.addChangeListener(change -> {
    // コールバック関数定義
    for (Result result : change.getResults()) {
        /*  例えばUI更新のように変更を反映するための処理を記述します */
    }
});

// クエリの実行
query.execute();
//}


ライブクエリを停止するには、リスナーを追加した際に受け取ったトークンを使用してリスナーを削除します。

//emlist[][java]{
query.removeChangeListener(token); 
//}

=== Android RecyclerView連携

Androidプラットフォームでは、動的なデータを効率的に表示するために、@<tt>{RecyclerView}@<fn>{RecyclerView}を用いることができます。

//footnote[RecyclerView][https://developer.android.com/reference/androidx/recyclerview/widget/RecyclerView]

以下に、Couchbase LiteをRecyclerViewのデータソースとして使用する際の一般的な構成を示します。


//image[live-query-android][Couchbase LiteとRecyclerViewとの関係]{
//}

(図は、Couchbaseチュートリアル Using Couchbase Lite with Recycler Views@<fn>{tutorials-university-lister-android}より引用)

//blankline

//footnote[tutorials-university-lister-android][https://docs.couchbase.com/tutorials/university-lister/android.html]


 * @<tt>{RecyclerView}は@<tt>{Activity}によってインスタンス化されます。@<tt>{RecyclerView}は@<tt>{Adapter}と関係を持っています。
 * @<tt>{Adapter}は、@<tt>{ViewHolder}を介してデータ項目をビューにバインドする役割を果たします。
 * @<tt>{DatabaseManager}は、Couchbase Liteの初期化と管理を担当します。
 * @<tt>{Activity}から、Couchbase Liteに対して、ライブクエリを構成するリスナーが登録されます。


//blankline

以下、チュートリアル Using Couchbase Lite with Recycler Views@<fn>{tutorials-university-lister-android}の内容をベースに、Androidアプリ開発におけるライブクエリの利用について、実際のコーディング内容を紹介します。


まず、以下のように@<tt>{Activity}の@<tt>{onCreate}メソッドで@<tt>{RecyclerView}の初期化が行われます。

//emlist[ListActivity.java][JAVA]{
@Override
protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    ...

    // RecyclerViewの設定
    RecyclerView recyclerView = (RecyclerView)findViewById(R.id.rvUniversities);
    recyclerView.setAdapter(adapter); 
    recyclerView.setLayoutManager(new LinearLayoutManager(this));

    ...
}
//}

ここで@<tt>{RecyclerView}に設定されている@<tt>{adapter}は、@<tt>{RecyclerView.Adapter<UniversityListAdapter.ViewHolder>}を継承したクラスです。

以下は、ライブクエリ登録箇所です。
@<tt>{University}オブジェクトのリストを扱うためにライブクエリを設定しています。

//emlist[ListActivity.java][JAVA]{
// クエリの構築
query = QueryBuilder.select(SelectResult.all()).
    from(DataSource.database(dbMgr.database)); 

// ライブクエリリスナーを追加
query.addChangeListener(new QueryChangeListener() { 
    @Override
    public void changed(QueryChange change) {
        ResultSet resultRows = change.getResults();
        Result row;
        List<University> universities = new ArrayList<University>();
        // リスナーコールバック内における、結果セットの反復処理
        while ((row = resultRows.next()) != null) { 

            Dictionary valueMap = row.getDictionary(dbMgr.database.getName()); 

            ObjectMapper objectMapper = new ObjectMapper();
            objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
            University university = objectMapper.convertValue(valueMap.toMap(),University.class);
            universities.add(university); 
        }

        // アダプターにデータをセット
        adapter.setUniversities(universities); 

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // アダプターへ通知
                adapter.notifyDataSetChanged(); 
            }
        });
    }
});

// クエリ実行
query.execute(); 
//}


