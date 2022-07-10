
= Couchbase Mobile競合解決


== 概要

Couchbase Mobileでは、自動競合解決機能が提供されています。競合が検出されるとデフォルトの競合解決ロジックが呼び出されます。
システムが競合を自動的に処理するため、開発者が競合に対処することは必須ではありません。
一方、開発者は、要件に応じて、独自の競合解決ロジックを実装することもできます。

この章では、最初に競合が発生するシナリオを整理します。
そして、それぞれのシナリオにおける、デフォルトの自動競合解決ロジックと、カスタム競合解決ロジックの実装方法およびその典型的な設計について解説します。

//footnote[memo-mode][「モード」という語は、(コンフリクトフリーではない)別のモードが存在することを暗に示す表現だといえます。この表現は、コンフリクトフリーモードがCouchbase Mobile 2.0から導入されたという経緯に由来しています。自動競合解決を利用しないオプションは、2.0よりも前のバージョンのクライアントが共存する状況のために用意されていると、本書では扱いません。]

なお、本章の記述はCouchbase Blog: Document Conflicts & Resolution in Couchbase Mobile 2.0@<fn>{document-conflicts-couchbase-mobile}を参考にしています。@<fn>{memo-disclaimer}

//footnote[memo-disclaimer][説明にあたって、ソフトウェア内部の挙動など適宜簡略化している部分があります。厳密な内容については参照源にあたってください。]

== 競合発生シナリオ


同じドキュメントに対して、複数のクライアントから「同時」に更新が行われる場合、競合が発生します。
ここで「同時」とは、「前のレプリケーションから次のレプリケーションまでの間」に対応します。
例えば、クライアントが一定期間オフラインになった後に、再度オンラインに復帰した場合には、オフライン期間中にそのクライアントによって行われた変更は、その間に他のすべてのクライアントによって行われた変更と実質的に「同時」に行われたものとみなされます。

=== Couchbase Liteでの競合検出

Couchbase Liteでの競合検出には、@<em>{データベース更新時}と@<em>{プルレプリケーション時}の二つがあります。

例えば、ドキュメントをローカルデータベースから取得し、変更を加えた上で保存する前に、そのドキュメントが、プルレプリケーションによってリモートデータベースの変更を反映して更新されていた場合、ドキュメントをローカルデータベースに保存しようとしたタイミングで競合が検知されます。

次の図は、そのシナリオを示しています。


//image[cbl-conflict-case][データベース更新時の競合発生シナリオ]{
//}

(図は、Couchbase Blog: Document Conflicts & Resolution in Couchbase Mobile 2.0@<fn>{document-conflicts-couchbase-mobile}より引用)

//blankline

また、ローカルデータベースのドキュメントを更新した後に、リモートデータベースで同じドキュメントへ行われていた変更がプルレプリケーションによって反映される場合は、レプリケーションプロセスの中で競合が検知されることになります。


=== Sync Gatewayでの競合検出

Sync Gatewayでの競合検出は、@<em>{プッシュレプリケーション時}に行われます。

複数のクライアントが、同じドキュメントに対して、それぞれ変更を加え、その変更がリモートデータベースに対して「同時」にプッシュされる場合、Sync Gateway側で競合が検知されます。

次の図は、そのシナリオを示しています。

//image[sg-conflict-case][プッシュレプリケーション時の競合発生シナリオ]{
//}

(図は、Couchbase Blog: Document Conflicts & Resolution in Couchbase Mobile 2.0@<fn>{document-conflicts-couchbase-mobile}より引用)


== データベース更新時の競合解決


=== デフォルトポリシー


Couchbase Liteデータベースに対してドキュメントの保存を行う場合、データベース中のドキュメントと競合が発生していたとしても、基本的には最後の書き込みが常に勝ちます。
つまり、ドキュメントを読みとってから更新するまでの間にプルレプリケーションによって行われたドキュメントの変更はすべて上書きされます。
これは、データベース内のドキュメントが削除されている場合であっても同様です。つまり、結果として削除されたドキュメントは復活することになります。

=== ドキュメント新規作成時の考慮点

デフォルトの競合解決ポリシーでは、最後の書き込みが常に優先されます。
これは、ドキュメントの新規作成を意図した操作であっても、データベースにすでに同じドキュメントIDを持つドキュメントが存在していた場合、既存のドキュメントの更新となることを意味します。

新規ドキュメント作成時には、本来競合は発生しないはずです。
一方で、ドキュメントIDの生成方法次第によって、別の場所で作成された同じIDを持つドキュメントがローカルデータベースに同期される、という可能性が存在し得ます。

ドキュメントID設計上、このような状況を避けることができない場合には、新規ドキュメントの作成が、意図せずに既存ドキュメントの更新となってしまわないように、カスタム競合解決を用いて、ドキュメントIDを変更して保存するといった対応を行う必要が生じます。


=== ConcurrentControlオプションによるカスタム競合解決

多くの場合、「最後の書き込みが常に勝つ」というデフォルトのポリシーで十分だと考えられますが、ドキュメント保存時の動作をカスタマイズする方法が提供されています。

その方法の一つは、ドキュメントの保存を実行する際に競合が発生していることを検知できるようにすることです。
具体的には、ドキュメントの保存実行時のオプションパラメーターである@<tt>{ConcurrentControl}引数に、@<tt>{failOnConflict}を指定します(デフォルトは、@<tt>{lastWriteWins}が指定された状態)。
この引数指定による@<tt>{save}メソッドコールの戻り値が@<tt>{false}であった場合、ドキュメントの保存が競合発生の結果として失敗したことを示します。

この機能を利用して、競合発生によりドキュメントの保存が失敗した場合には、現在保存されているドキュメントを取得し、要件に応じたロジックに基づき変更を加えた上で、再度ドキュメントを保存する等の処理を実装することによって、競合解決ロジックをカスタマイズすることができます。

ただし、この方法によって競合の発生を検知した後に再度ドキュメントを保存する場合、再度競合状態が発生している可能性があることに注意を払う必要があります。
この問題への典型的な解決策は再試行のためのループを導入することです。

なお、ドキュメントの保存だけではなく削除の場合(@<tt>{delete}メソッド)にも、@<tt>{ConcurrentControl}引数を用いることができます。


@<tt>{ConcurrentControl}オプションを用いた実装について、Couchbase Blog: Document Conflicts & Resolution in Couchbase Mobile 2.0@<fn>{document-conflicts-couchbase-mobile}に、様々なパターンのプログラミング例が紹介されています。

//footnote[document-conflicts-couchbase-mobile][https://blog.couchbase.com/document-conflicts-couchbase-mobile/]


=== ConflictHandlerブロックによるカスタム競合解決

ドキュメント保存時にカスタム競合解決を実装する別の方法として、ConflictHandlerブロックを指定する方法が提供されています。
以下、その利用例を見ていきます。


競合をどのように処理するかは、アプリケーションの仕様によって異なります。
一つの方法として、競合が発生しているドキュメントをマージすることが考えられます。
以下は、ConflictHandlerブロックを使って、競合が発生しているドキュメントをマージする例です。


//emlist[][java]{
database.save(
    mutableDocument,
    (newDoc, curDoc) -> { 
        if (curDoc == null) { return false; } 
        Map<String, Object> dataMap = curDoc.toMap();
        dataMap.putAll(newDoc.toMap()); 
        newDoc.setData(dataMap);
        return true; 
    }); 
//}

マージをどのように処理するかはアプリケーションの仕様によって異なりますが、この例では、保存されているドキュメントに新しいドキュメントのプロパティを上書きしています。
現在保存されているドキュメントの内容と新たに保存しようとしているドキュメントの内容とを比較した上で、その結果に応じた処理を行うこともできます。

あるいは、競合発生時においてローカルの更新よりもリモートの更新を優先する場合は、単にそのドキュメントの保存をスキップすることも考えられます。

== プッシュレプリケーション時の競合解決


=== デフォルトポリシー


プッシュレプリケーション中に、競合を引き起こす更新のリクエストが行われた場合、Sync Gatewayはこのリクエストを拒否してHTTP 409エラーを返します。
これによって、Couchbase Serverデータベース上に競合するリビジョンがないことが保証されます。

Sync Gatewayから、409エラーが返された場合、レプリケーターは、それをログに記録する以外に何も行いません。
拒否されたリクエストに内在している競合は、プルレプリケーション中にクライアント側で処理されます。

====[column]Sync Gatewayのカスタム競合解決

エンタープライズエディションでは、Sync Gatewayにカスタム競合解決ロジックを実装することが可能になっています。
これは、構成プロパティ@<tt>{custom_conflict_resolver}に、JavaScript関数を定義することによって行われます。
ドキュメント@<fn>{custom-conflict-resolver}にその定義例が紹介されています。

====[/column]


//footnote[custom-conflict-resolver][https://docs.couchbase.com/sync-gateway/current/glossary.html#custom-conflict-resolver]


== プルレプリケーション時の競合解決


=== デフォルトポリシー

プルレプリケーション中に競合が検出された場合、競合は次の基準により解決されます。

//blankline

 * 更新が削除である場合、常に勝ちます。
 * 最新のリビジョン/最新の変更が勝ちます。@<fn>{memo-revision-win}

//blankline

このように、レプリケーションにおける競合はプルレプリケーション時にCouchbase Lite側で解決されます。
このことは、レプリケーターをプッシュレプリケーション専用に構成した場合、競合状態が解決される機会がなく、ローカルデータとリモートデータの間に差異が存続し続ける可能性があることを意味します。
したがって、このような競合が発生するユースケースでは、プッシュアンドプルレプリケーションを行うことが必要になります。

//footnote[memo-revision-win][ここでは、ソフトウェア内部におけるデータ管理仕様の詳細に立ち入らず簡潔な記述に留めていますが、興味がある場合は、より具体的な内容について本章解説の参照源として紹介したCouchbase Blog: Document Conflicts & Resolution in Couchbase Mobile 2.0を参照することができます。]

=== ConflictResolverによるカスタム競合解決

Couchbase Liteのレプリケーターに対して、カスタム競合解決を実装するためのConflictResolver@<fn>{replicator-conflict-resolver}インターフェイスが提供されています。

//footnote[replicator-conflict-resolver][https://docs.couchbase.com/couchbase-lite/2.6/java.html#conflict-resolver]


利用する場合には、@<tt>{resolve}メソッドを実装したクラスを、レプリケーター構成オブジェクトに登録します。
@<tt>{ConflictResolver}を設定しない場合には、デフォルトの競合解決が適用されます。

//emlist[][java]{
ReplicatorConfiguration config = new ReplicatorConfiguration(database, target);
config.setConflictResolver(new LocalWinConflictResolver());

Replicator replication = new Replicator(config);
replication.start();
//}


以下に、カスタム競合レゾルバーの実装例を紹介します。

まず以下のように、マージされた新しいドキュメントを返す実装が可能です。

//emlist[][java]{
class MergeConflictResolver implements ConflictResolver {
    public Document resolve(Conflict conflict) {
        Map<String, Object> merge = conflict.getLocalDocument().toMap();
        merge.putAll(conflict.getRemoteDocument().toMap());
        return new MutableDocument(conflict.getDocumentId(), merge);
    }
}
//}


以下は、競合発生時に、常にリモートドキュメントを採用する例です。

//emlist[][java]{
class RemoteWinConflictResolver implements ConflictResolver {
    public Document resolve(Conflict conflict) {
        return conflict.getRemoteDocument();
    }
}
//}


以下は、競合発生時に、常にローカルドキュメントを採用する例です。

//emlist[][java]{
class LocalWinConflictResolver implements ConflictResolver {
    public Document resolve(Conflict conflict) {
        return conflict.getLocalDocument();
    }
}
//}


