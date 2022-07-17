= Couchbase Liteツール

== cblite

=== 概要

Couchbase Liteデータベースファイルを検査したり、データベースファイルに対してクエリを実行することができるコマンドラインツールとして、@<tt>{cblite} があります。
このコマンドはトラブルシューティングに用いられる他、開発者がアプリにビルトインするためのデータベースを準備するために利用することができます。


@<tt>{cblite}は、Couchbase LabsのGitHubリポジトリー The Couchbase Mobile Tool Repo@<fn>{couchbase-mobile-tools}から入手することができます。


//footnote[couchbase-mobile-tools][https://github.com/couchbaselabs/couchbase-mobile-tools/]

=== サブコマンド

@<tt>{cblite}は、サブコマンドを用いて様々な用途に用いることができます。

以下に、サブコマンドの概要を示します。

//blankline

//table[tbl1][]{
コマンド	目的
-----------------
@<tt>{cat}	データベース内のドキュメントの内容を表示します。
@<tt>{check}	データベースファイルの整合性、破損の有無をチェックします。
@<tt>{compact}	データベースファイルを圧縮します。
@<tt>{cp}	データベースの複製、インポート、エクスポートを行います。
@<tt>{decrypt}	データベースを複合化します。
@<tt>{encrypt}	データベースを暗号化します。
@<tt>{help}	ヘルプテキストを表示します。
@<tt>{info}	データベースに関する情報を表示します。
@<tt>{logcat}	バイナリログファイルをプレーンテキストに変換します。
@<tt>{ls}	データベース内のドキュメントを一覧表示します。
@<tt>{mv}	ドキュメントを現在のコレクションから別のコレクションへ移動します。
@<tt>{mkcoll}	コレクションを作成します。
@<tt>{open}	インタラクティブモードでデータベースを開きます。
@<tt>{openremote}	リモートデータベースを一時的にローカルにプルし、インタラクティブモードで開きます。
@<tt>{put}	ドキュメントを作成または更新します。
@<tt>{query}	JSONクエリスキーマを使用してクエリを実行します。
@<tt>{reindex}	インデックスを再構築します。
@<tt>{revs}	ドキュメントの改訂履歴を一覧表示します。
@<tt>{rm}	ドキュメントを削除します。
@<tt>{select}	SQL++/N1QL構文を使用してクエリを実行します。
@<tt>{serve}	簡易的なREST APIリスナーを開始します。
//}

//blankline

この他、@<tt>{cp}に類似する機能を提供するコマンドとして、@<tt>{push}、@<tt>{export}、@<tt>{pull}、@<tt>{import}があります。
ここでは、@<tt>{cp}コマンドに代表させています。


=== 実行モード


@<tt>{cblite}は、以下のふたつの実行モードで利用することができます。

//blankline

 * インタラクティブ(対話)モード: データベースパスを指定して実行後、インタラクティブにサブコマンドの入力を行います。
 * ワンショット（非インタラクティブ）モード: 最初の引数としてサブコマンドを、その後にサブコマンドに応じたオプションの引数を指定して実行します。

//blankline

以下は、インタラクティブモードでの実行例です。


//cmd{
$ cblite travel-sample.cblite2
Opened read-only database ./travel-sample.cblite2/
(cblite) ls -l --limit 5

airport_1254            1-d4d718ab ---     918     0.2K
airport_1255            1-bf36065e ---     919     0.2K
airport_1256            1-3bd3788b ---     920     0.2K
airport_1257            1-6298d53f ---     921     0.2K
airport_1258            1-11b49ddf ---     922     0.2K
(Stopping after 5 docs)
(cblite) quit
//}

インタラクティブモードを終了する際は、上記のように@<tt>{quit}を入力します。

=== グローバルフラグ


グローバルフラグは @<tt>{cblite}コマンドの直後に指定します。以下のような機能を提供します。

//blankline

//table[tbl2][]{
フラグ	効果
-----------------
@<tt>{--color}	ANSIカラーを有効にします。
@<tt>{--create}	新しいデータベースを作成します。作成されたデータベースは書き込み可能モードでオープンされます。
@<tt>{--encrypted}	暗号化されたデータベースをオープンします。暗号化キーの入力が求められます。なお、インタラクティブモードでは、データベースが暗号化されていることが検出されると自動的にプロンプ​​トが表示されます。
@<tt>{--upgrade}	データベースを開く際に、データベースバージョンのアップグレードを許可します。アップグレードされたデータベースは下位互換性を失います。
@<tt>{--version}	バージョンを出力します。
@<tt>{--writeable}	データベースを書き込み可能モードでオープンします。
//}

== cbliteサブコマンド

@<tt>{cblite}のサブコマンドについて、開発やトラブルシューティングに際して特に有益と思われるものを選んで、利用可能なフラグや利用例を紹介します。

@<tt>{check}や@<tt>{compact}のように、開発やトラブルシューティングに利用できるものであっても、追加のフラグやパラメーターが存在せず、特に解説を要しないものは扱っていません。
上掲のサブコマンド表も参考ください。

全てのサブコマンドの詳細については、ドキュメント@<fn>{cblite-documentation}を参照ください。


//footnote[cblite-documentation][https://github.com/couchbaselabs/couchbase-mobile-tools/blob/master/Documentation.md]

=== 凡例

以下ではサブコマンド毎に、まずはじめにワンショットモードとインタラクティブモードのそれぞれについて、利用時の構文構造を示します。

そこで用いられているキーワードの意味を記します。

//blankline

 * @<b>{flags}: サブコマンドに指定するフラグ
 * @<b>{databasepath}: データベースファイルパス
 * @<b>{DOCID}: ドキュメントID

//blankline

@<b>{[flags]}のような、角括弧内の記載はオプションであることを表します。

=== cat

@<tt>{cblite cat} @<b>{[flags]} @<b>{databasepath} @<b>{DOCID} [@<b>{DOCID} ...]

//blankline


@<tt>{cat} @<b>{[flags]} @<b>{DOCID} [@<b>{DOCID} ...]

//blankline


ドキュメントID指定により、ドキュメントのJSON本文を表示します。

ドキュメントIDの指定(@<b>{DOCID})には、シェルスタイルのワイルドカード(「@<tt>{*}」、「@<tt>{?}」)を用いることができます。
この場合、パターンに一致した全てのドキュメントが表示されます。


//blankline

//table[tbl3][]{
フラグ	効果
-----------------
@<tt>{--key KEY}	指定したキー(ドキュメントプロパティー)とその値のみを表示します。複数個同時に指定できます。
@<tt>{--rev}	リビジョンIDを表示します。
@<tt>{--raw}	JSONを、そのまま出力します。
@<tt>{--json5}	JSON5構文で出力します(JSON5構文では、キーが引用符で囲まれません)。
//}


=== cp

@<tt>{cblite cp} @<b>{[flags]} @<b>{source} @<b>{destination}

//blankline

@<tt>{cp} @<b>{[flags]} @<b>{destination}

//blankline

@<b>{source}から@<b>{destination}に対してデータを移動します。

インタラクティブモードでは、@<tt>{cblite}コマンド実行時に指定したデータベースが@<b>{source}として使用され、@<tt>{cp}サブコマンドは@<b>{destination}のみを引数として取ります。


@<b>{source}と@<b>{destination}は、データベースファイルや、JSONファイル等、多様な形態を取ることができます。
ただし、@<b>{source}または@<b>{destination}のいずれかが、@<tt>{.cblite2}で終わるデータベースパスである必要があります。

データを移動する方法は、@<b>{source}と@<b>{destination}の指定によって決まります。
指定方法の違いにより、データベースへのJSONドキュメントのインポート、またはデータベースからのJSONドキュメントのエクスポートを行うことができます。

なお、@<tt>{cp}サブコマンドでは、Couchbase Mobileの重要な機能のひとつであるふたつのデータベース間のデータ同期(レプリケーション)操作についてもカバーされていますが、ここではレプリケーションに関連する記述については割愛しています。必要に応じ、ドキュメント@<fn>{cblite-documentation}を参照ください。

@<b>{source}と@<b>{destination}は、次のいずれかの形式で指定します。

//blankline

 * @<tt>{.cblite2}で終わるデータベース
 * @<tt>{.json}で終わるJSONファイル。1行にひとつのJSONドキュメントを含むファイルである必要があります。
 * @<tt>{/}で終わるディレクトリー。ディレクトリーにはJSONファイル(ドキュメントごとに1ファイル)が含まれている必要があります。

//blankline


//table[tbl4][]{
フラグ	効果
-----------------
@<tt>{--careful}	エラーが発生した場合は中止します。
@<tt>{--existing}または@<tt>{-x}	@<b>{destination}がまだ存在しない場合は失敗します。
@<tt>{--jsonid} @<b>{property}	ドキュメントIDに使用するJSONプロパティーを指定します。詳細、下記参照ください。
@<tt>{--limit} @<b>{n}	@<b>{n}個のドキュメントに対する実行の後で停止します。
@<tt>{--verbose}または@<tt>{-v}	進捗情報をログに記録します。
//}

//blankline

@<tt>{--jsonid}フラグは次のように機能します。

//blankline

 * @<b>{source}がJSONの場合、ドキュメントIDとして使用するプロパティー名を指定します。省略した場合、ドキュメントIDにはUUIDが用いられます。
 * @<b>{destination}がJSONの場合、ドキュメントIDを追加するためのJSONのプロパティー名を指定します。このフラグを省略すると、プロパティー名はデフォルトで @<tt>{_id}になります。


=== info

@<tt>{cblite info} @<b>{databasepath} 

@<tt>{cblite info} @<b>{databasepath} @<tt>{indexes} 

@<tt>{cblite info} @<b>{databasepath} @<tt>{index} @<b>{indexname}


//blankline

@<tt>{info} 

@<tt>{info indexes} 

@<tt>{info index} @<b>{indexname} 

//blankline

データベースのサイズや格納されているドキュメント数など、データベースに関する情報を表示します。 
サブサブコマンド @<tt>{indexes}を使用すると、データベース内のすべてのインデックスが一覧表示されます。 
サブサブコマンド @<tt>{index}の後にインデックス名を付けると、そのインデックスの内容（キーと値）がダンプされます。

//blankline

以下に、実行例を示します。

//cmd{
$ cblite info travel-sample.cblite2
Database:    /path/to/travel-sample.cblite2
Size:        6.4MB  (use -v for more detail)
Collections: "_default": 2885 documents, last sequence #2885
//}


=== logcat


@<tt>{cblite logcat} @<b>{logfile} [@<b>{logfile} ...]

@<tt>{cblite logcat} @<b>{directory}


//blankline

@<tt>{logcat} @<b>{logfile} [@<b>{logfile} ...]

@<tt>{logcat} @<b>{directory}

//blankline

Couchbase Liteのバイナリログファイルを読み取り、プレーンテキストとして出力します。

ログファイル(@<b>{logfile})を複数指定すると、出力はマージされ時系列で並べ替えられます。

ディレクトリーパス(@<b>{directory})を指定すると、そのディレクトリー内のすべての @<tt>{.cbllog}ファイルが読み取られます。

なお、このサブコマンドは、@<b>{databasepath}を必要としません。

//blankline


//table[tbl5][]{
フラグ	効果
-----------------
@<tt>{--csv}	CSV形式で出力します。
@<tt>{--full}	すべてのログが読み込まれた後に、ログレベル毎に重要性の高い順に出力が開始されます。
@<tt>{--out} @<b>{filepath}	標準出力ではなくファイルに出力を書き込みます。
//}

=== ls


@<tt>{cblite ls} @<b>{[flags]} @<b>{databasepath} @<b>{[PATTERN]}

//blankline

@<tt>{ls} @<b>{[flags]} @<b>{[PATTERN]}

//blankline

データベース内のドキュメントを一覧表示します。フラグを指定しない場合ドキュメントIDのみが表示されます。



@<b>{[PATTERN]}は、シェルスタイルのワイルドカード「@<tt>{*}」、「@<tt>{?}」を使用してドキュメントIDを照合するためのオプションの引数です。


//blankline

//table[tbl6][]{
フラグ	効果
-----------------
@<tt>{-l}	1行にひとつのドキュメントの詳細情報を表示します。
@<tt>{--offset} @<b>{n}	最初の@<b>{n}個のドキュメントをスキップします。
@<tt>{--limit} @<b>{n}	@<b>{n}個のドキュメントを表示した後に停止します。
@<tt>{--desc}	降順で表示します。
@<tt>{--seq}	ドキュメントIDではなくシーケンスで並べ替えます。
@<tt>{--del}	削除されたドキュメントを含めます。
@<tt>{--conf}	競合するドキュメントのみを含めます。
@<tt>{--body}	ドキュメント本文を表示します。
@<tt>{--pretty}	ドキュメント本文を読みやすい形で表示します（ @<tt>{--body}と同時に利用）。
@<tt>{--json5}	JSON5構文、つまりキーを引用符で囲まずに表示します(@<tt>{--body}と同時に利用）。
//}



以下に、オプションを組み合わせて実行した例を示します。

//cmd{
$ cblite ls -l --limit 10 travel-sample.cblite2

airport_1254            1-d4d718ab ---     918     0.2K
airport_1255            1-bf36065e ---     919     0.2K
airport_1256            1-3bd3788b ---     920     0.2K
airport_1257            1-6298d53f ---     921     0.2K
airport_1258            1-11b49ddf ---     922     0.2K
airport_1259            1-6f41e72f ---     923     0.2K
airport_1260            1-a57d8cf5 ---     924     0.2K
airport_1261            1-6bdfcdda ---     925     0.2K
airport_1262            1-9aeab53b ---     926     0.2K
airport_1263            1-2cf49f25 ---     927     0.1K
(Stopping after 10 docs)
//}

=== put


@<tt>{cblite put} @<b>{[flags]} @<b>{databasepath} @<b>{DOCID} "@<b>{JSON}"


//blankline

@<tt>{put} @<b>{[flags]} @<b>{DOCID} "@<b>{JSON}"

//blankline

JSON文字列(@<b>{JSON})を指定して、ドキュメントを作成または更新します。

//blankline

//table[tbl7][]{
フラグ	効果
-----------------
@<tt>{--create}	ドキュメントを作成します。ドキュメントが存在する場合は失敗します。
@<tt>{--update}	既存のドキュメントを更新します。ドキュメントが存在しない場合は失敗します。
//}

//blankline

インタラクティブモードでは、 データベースが書き込み可能モードでオープンされていない(@<tt>{--writeable}または @<tt>{--create}フラグが用いられていない)場合、このコマンドは失敗します。


=== query

//blankline

@<tt>{cblite query} @<b>{[flags]} @<b>{databasepath} @<b>{query}


//blankline

@<tt>{query} @<b>{[flags]} @<b>{query}


//blankline

データベースへのクエリを実行します。
@<b>{query}の記述は、JSONクエリスキーマ@<fn>{json5}に従います。

//blankline

//table[tbl8][]{
フラグ	効果
-----------------
@<tt>{--explain}	クエリプランを出力します。
@<tt>{--offset} @<b>{n}	最初の@<b>{n}行をスキップします。
@<tt>{--raw}	(テーブル形式ではなく)JSONデータを出力します。
@<tt>{--limit} @<b>{n}	@<b>{n}行の後で停止します。
//}

以下に、実行例を示します。

//cmd{
$ cblite travel-sample.cblite2
Opened read-only database ./travel-sample.cblite2/
(cblite) query {"FROM":[{"COLLECTION":"_"}],"GROUP_BY":[[".state"]],"ORDER_BY":[[".num"]],"WHAT":[[".state"],["AS",["COUNT()",["."]],"num"]],"WHERE":["=",[".type"],"hotel"]}
state                       num
___________________________ ___ 
Aquitaine                     1
Auvergne                      1
Bourgogne                     1
Bretagne                      1
Midi-Pyrénées               1
Nord-Pas-de-Calais            1
Basse-Normandie               2
Corse                         3
Haute-Normandie               4
Rhône-Alpes                 10
Provence-Alpes-Côte d'Azur  33
Île-de-France               78
California                  361
null                        420
//}


//footnote[json5][http://json5.org]



=== reindex

@<tt>{cblite reindex} @<b>{databasepath}


//blankline

@<tt>{reindex}

//blankline


インデックスを再構築します。

再構築後のインデックスは、段階的に更新されてきたインデックスよりも効率的な構造になる可能性があるため、クエリのパフォーマンスが向上する可能性があります。
アプリケーション内に埋め込むデータベースを準備する際の最終ステップとして実行する価値があります。


インタラクティブモードでは、 データベースが書き込み可能モードでオープンされていない(@<tt>{--writeable}または @<tt>{--create}フラグが用いられていない)場合、このコマンドは失敗します。


=== select


@<tt>{cblite select} @<b>{[flags]} @<b>{databasepath} @<b>{query}

//blankline

@<tt>{select} @<b>{[flags]} @<b>{query}

//blankline

データベースへのクエリを実行します。
@<b>{query}の記述は、SQL++/N1QL構文に従います。

//blankline

//table[tbl9][]{
フラグ	効果
-----------------
@<tt>{--explain}	クエリプランを出力します。
@<tt>{--ofset} @<b>{n}	最初の@<b>{n}行をスキップします。
@<tt>{--raw}	(テーブル形式ではなく)JSONデータを出力します。
@<tt>{--limit} @<b>{n}	@<b>{n}行の後で停止します。
//}

//blankline

@<b>{query}記述のベースは、SQL++/N1QLの @<tt>{SELECT}クエリです。

キーワード@<tt>{select}はコマンド名に含まれているため、@<b>{query}の記載から除きます。

以下に、実行例を示します。

//cmd{
$ cblite travel-sample.cblite2
Opened read-only database ./travel-sample.cblite2/
(cblite) select name from _ where type = 'hotel' order by name limit 10
'La Mirande Hotel
192 B&B
500 West Hotel
54 Boutique Hotel
8 Clarendon Crescent
AIRE NATURELLE LE GROZEAU Aire naturelle
Abbey Hotel
Aberdovey Hillside Village
Ace Hotel DTLA
Adagio La Défense Esplanade
(cblite) 
//}


== クエリ調査

Couchbase Liteで用いられるクエリの調査またはトラブルシューティング(Query Troubleshooting@<fn>{query-troubleshooting})方法について解説します。


調査方法には、@<tt>{cblite}コマンドを利用する方法とAPIを利用する方法があります。
これらのいずれの方法を用いた場合でも、出力内容は同じフォーマットとなります。
ここでは、@<tt>{cblite}コマンドを用いる方法について説明します。

出力内容は、クエリのパフォーマンスの問題を診断したり、クエリを最適化したりするときに役立つ情報を提供します。


=== 実行方法


ターミナル上で、データベースを指定して、インタラクティブモードで、@<tt>{cblite}コマンドを実行します。

データベースへのクエリを行うための@<tt>{cblite}のサブコマンドとして、@<tt>{select}と@<tt>{query}があります。
クエリを調査する場合、これらのサブコマンドに対して、@<tt>{--explain}オプションを指定します。

次の例では、@<tt>{select}サブコマンドを使用しています。 

//cmd{
(cblite) select --explain state, COUNT(*) AS num from _ where type = 'hotel' group by state order by num
//}


=== 出力内容

@<tt>{--explain}オプションを指定してクエリを実行した場合の出力内容は、以下のようなフォーマットになります。


//cmd{
SELECT fl_result(fl_value(_.body, 'state')), fl_result(count(fl_root(_.body))) AS num FROM kv_default AS _ WHERE (fl_value(_.body, 'type') = 'hotel') AND (_.flags & 1 = 0) GROUP BY fl_value(_.body, 'state') ORDER BY num

7|0|0| SCAN TABLE kv_default AS _
15|0|0| USE TEMP B-TREE FOR GROUP BY
57|0|0| USE TEMP B-TREE FOR ORDER BY

{"FROM":[{"COLLECTION":"_"}],"GROUP_BY":[[".state"]],"ORDER_BY":[[".num"]],"WHAT":[[".state"],["AS",["COUNT()",["."]],"num"]],"WHERE":["=",[".type"],"hotel"]}
//}


出力は、次の3つの主要な要素で構成されています。それぞれの要素は空白行で区切られます。

//blankline

 * SQLクエリに翻訳された文字列: あくまで診断用の情報であり、必ずしもそのまま実行可能なフォーマットとは限らないことに注意してください。
 * クエリプラン: クエリ実行に関するハイレベルの情報を提供します。
 * JSON文字列形式のクエリ: @<tt>{cblite}ツールの@<tt>{query}サブコマンドで直接実行することができます。

//blankline

なお、Couchbase Liteは内部的にSQLiteを利用しており、クエリプランの出力はSQLiteの@<tt>{EXPLAIN QUERY PLAN}コマンドに関係します。
SQLiteの@<tt>{EXPLAIN QUERY PLAN}の詳細については、SQLiteドキュメント@<fn>{sqlite-eqp}を参照ください。


//footnote[query-troubleshooting][https://docs.couchbase.com/couchbase-lite/current/objc/query-troubleshooting.html]


//footnote[sqlite-eqp][https://www.sqlite.org/eqp.html]

=== クエリプラン


出力中のクエリプランセクションには、クエリの実行プランが表形式で表示されます。
出力内容は、データがどのように取得され、どのように加工されるかを示しています。

以下に出力例を示します。

//cmd{
7|0|0| SCAN TABLE kv_default AS _
15|0|0| USE TEMP B-TREE FOR GROUP BY
57|0|0| USE TEMP B-TREE FOR ORDER BY
//}

各行は、下記の内容に対応しています。

１行目は、クエリに使用されている検索方法(Retrieval method)を示しています。上の例では、データベースの順次読み取り(SCAN)が行われており、最適化を検討できる可能性があります。検索方法(Retrieval method)には、以下の三種類があります。

//blankline

 * SEARCH: キーを使用して必要なデータに直接アクセスできます。 最速の検索方法です。
 * SCAN INDEX: インデックスをスキャンすることでデータを取得します。SEARCHよりも低速ですが、インデックスの恩恵を受けます。
 * SCAN TABLE: データベースをスキャンして、必要なデータを取得する必要があります。最も遅い検索方法であり、最適化を検討することが考えられます。

//blankline

２行目はグループ化方法(Grouping method)を、３行目は順序付け方法(Ordering method)を示しており、それぞれBツリーが一時ストレージとして使用されています。

== Visual Studio Codeプラグイン

=== 概要

Visual Studio Code(VSCode)用のプラグインとして、Couchbase Lite for VSCode@<fn>{visualstudio-code}が提供されています。

マーケットプレイスにて、「couchbase」で検索して、インストールすることができます。

//image[vscode-marketplace][Visual Studio Codeマーケットプレイス画面]{
//}

このプラグインを使うことによって、VSCode上でCouchbase Liteデータベースファイルに対して、データの閲覧、編集、そしてクエリが可能になります。

//footnote[visualstudio-code][https://code.visualstudio.com]

//footnote[vscode-cblite][https://github.com/couchbaselabs/vscode-cblite]

このプラグインのソースコードは、Couchbase LabsのGitHubリポジトリーvscode-cblite@<fn>{vscode-cblite}で公開されています。
最新のアップデートや利用方法について、このリポジトリーのREADMEを参照することができます。

=== データベース内容の確認

プラグインの利用を開始するには、はじめにCouchbase LiteデータベースをVSCodeのサイドパネルのエクスプローラー画面で開きます。
ここで、選択するCouchbase Liteデータベースは「〜.cblite2」という名称のフォルダーになります(そのフォルダーの中にあるファイルを選択するのではないことにご注意ください)。

エクスプローラー画面にCouchbase Liteデータベースが表示されたら、それを選択し、右クリックメニューから[Open Database]へ進みます。

データベースがオープンされると、エクスプローラー画面の下部に[CBLITE EXPLORER]が表示されます。
この[CBLITE EXPLORER]には、データベースに格納されているドキュメントとその内容がツリービューで表示されます。

ドキュメントの内容をJSON形式で表示するには、ツリービューでドキュメントを選択して、右クリックメニューから[Get Document]を選びます。
すると、エディター画面にJSONドキュメントが１行で表示されます。エディター画面上で右クリックメニューから[Format Document]を選択することで、プロパティー毎に改行されたフォーマットでドキュメントの内容を表示することが可能です。

=== ドキュメントの更新

ドキュメントを更新するには、まずエディター画面に表示されたJSONデータを編集します。

データベースに対して、編集したドキュメントの更新を反映するためには、VSCodeのコマンドパレット([F1]キー、またはWindows/Linuxでは[Ctrl]+[Shift]+[P]、macOSでは[Command]+[Shift]+[P]で表示)から、次のコマンドを実行します。

//blankline

@<tt>{cblite: Update Document}

=== クエリ

データベースに対してクエリを行うには、[CBLITE EXPLORER]でデータベースを選択し、右クリックメニューから[New SQL++ Query]を選択します。

エディター画面には、はじめサンプルクエリが表示されるので、実行したいクエリに変更します。

クエリを実行するには、VSCodeのコマンドパレットを利用して、下記のコマンドを実行します。

//blankline

@<tt>{cblite: Run Query}
