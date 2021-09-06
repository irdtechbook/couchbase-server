
= 環境設定

Couchbase Serverの環境設定について、特に分散アーキテクチャーやCouchbase Serverに特有の概念や機能に関係するものを解説します。

網羅的な解説は本書で扱える範囲を超えるため、適宜ドキュメント@<fn>{manage-settings}を参照ください。

//footnote[manage-settings][https://docs.couchbase.com/server/current/manage/manage-settings/manage-settings.html]

== コンパクション

=== メカニズム

Couchbase Serverにおけるデータの挿入、更新、削除は全て、物理的にディスク上のデータファイルに対する「追加」書き込みとして実現されます。
この結果として生じるデータファイル内の(特にデータが削除された場合に顕著な)ギャップは、コンパクションと呼ばれるプロセスを通じて解消されます。

コンパクションの際には、新しいファイルが作成されます。その間、既存のデータベースファイルはそのまま残り、情報の格納やインデックスデータの更新に引き続き使用されます。このプロセスにより、コンパクションが行われている間もデータベースが引き続き使用可能になります。
コンパクションが完了すると、古いデータベースファイルが無効になります。その後、新しく作成されたデータベースファイルで受信更新が続行されます。その後、古いデータベースファイルがシステムから削除されます。

コンパクションは、Couchbase Serverの実行中にバックグラウンドプロセスとして実行されます。
データベース操作をシャットダウンまたは一時停止する必要はありません。

Couchbase Serverではコンパクションは自動的に実行（オートコンパクション）されます。また、REST APIコールを用いて、コンパクションをマニュアルで実行することも可能です。

=== 運用上の留意点

コンパクションの運用にあたり、下記の要因に注意を払う必要があります。

 * @<strong>{余裕のあるリソース状況} コンパクションプロセスはディスクとCPUの両方を集中的に使用します。

 * @<strong>{十分なディスク領域} コンパクションは新しいファイルを作成するため、コンパクションを行うために現在のデータベースファイルのディスク領域の2倍が必要になる場合があります。コンパクションが実行される前に、ディスク領域がチェックされます。使用可能なディスクスペースが現在のデータベース・サイズの2倍未満の場合、コンパクション処理は行われず、ログに警告が出されます。

=== オートコンパクション

Couchbase Serverで、オートコンパクションが実行されるトリガーには、下記の２つの要素があります。

 * @<strong>{データベースの断片化} これがプライマリ設定です。この数値を10%に設定すると、データファイルが10%断片化された時点でオートコンパクションがトリガーされます(オートコンパクション実行の時間帯制限がない限り)
 * @<strong>{時間帯} データベースへが多用されているときにオートコンパクションが行われないようにするには、オートコンパクションを許可する時間帯を設定します。これは、時・分の単位で設定できます（たとえば、01:00から 06:00の間に実行）。デフォルトでは、コンパクションは順次実行されます。


=== 参考情報

Couchbase公式ドキュメント Compaction API@<fn>{compaction-rest-api}

Couchbase公式ドキュメント Performing Compaction Manually@<fn>{rest-compact-post}

//footnote[compaction-rest-api][https://docs.couchbase.com/server/current/rest-api/compaction-rest-api.html]

//footnote[rest-compact-post][https://docs.couchbase.com/server/current/rest-api/rest-compact-post.html]



== 代替アドレス

Couchbase Serverでは、クラスターを構成するノードに対して、「プリンシパルアドレス」に加えて、「代替(Alternate)アドレス」を割り当てることができます（また、代替ポート番号をそのノードで実行されているサービスに割り当てることもできます）。プリンシパルアドレスは、クラスターの初期構成時に設定されるアドレスです。代替アドレスの設定は必要に応じて行われます。代替アドレスの設定はCLIまたはREST APIで実行可能です。

代替アドレス設定の重要性・必要性を把握するためには、分散アーキテクチャーを持つクラスターにおいては、モノリシックなアーキテクチャであれば考慮する必要がない面を考慮する必要があることを理解する必要があります。
Couchbase Serverでは、クライアント/アプリケーションがSDKを介してクラスターに接続する際には、クラスターマップという要素が介在します。初期接続は、アプリケーションに設定されているノード（複数指定可能）に対して行われますが、接続確立（ブートストラップ）時に、接続したノードからクラスターマップを受け取り、以降の接続は、対象となるノードに対して直接行われます。つまり、ブートストラップ接続先への一方方向のリクエストだけを考えて、IPやドメイン名の解決が外部で整合性が取れていれば良いという訳にはいかない、ということです。

代替アドレスが使われる2つのパターンを以下に示します。
ここではクライアントからの接続に焦点を当てていますが、プリンシパルアドレスは、クライアントからの接続のみではなく、クラスター内のノード間の接続にも利用されることは、理解しておく価値があります。


=== 外部ネットワークからの接続

まず注意しておくと、この図ではアプリケーションが外部ネットワークにあり、内部ネットワークにあるCouchbase Serverを利用していますが、これは必ずしも典型的なケースではないことにご留意ください。一般的には、アプリケーションとCouchbase Serverは同じ内部ネットワークにあり、その場合、アプリケーションとCouchbase Serverとの接続には、プライベートIPが使われれば事足ります。この場合、代替アドレスを設定する必要はありません。

外部ネットワークにあるアプリケーションとの接続に限らず、複数の異なるネットワークにあるクラスター間でXDCRを実行する場合も、このケースに該当します。

//image[nat][]{
//}

このケースでは、クラスターの構築時（プリンシパルアドレス）には、プライベートIPアドレスが利用されます。
外部からの参照のために、ネットワークアドレス変換（NAT）機能で、各サーバーにパブリックIPを付与し、それらを代替アドレスに割り当てます。


=== デュアルネットワーク

次の図で示すように、セキュリティを最適化するために、デュアルネットワークを使用して複数のアプリケーションのノードへのアクセスを分離することが考えられます。

//image[dual_network][]{
//}

ここでは、2つのサーバーがあり、それぞれプライマリIPおよびセカンダリIPアドレスを持っています。それぞれのアドレスを、プリンシパルアドレスと代替アドレスに割り当てています。そして、２つのアプリケーションは、それぞれプリンシパルアドレス、代替アドレスのいずれかのみを利用します。


=== 参考情報

Couchbase公式ドキュメント Alternate Addresses@<fn>{alternate-addresses}

Couchbase公式ドキュメント REST APIリファレンス Managing Alternate Addresses@<fn>{rest-set-up-alternate-address}

Couchbase公式ドキュメント CLIリファレンス setting-alternate-address@<fn>{couchbase-cli-setting-alternate-address}

//footnote[alternate-addresses][https://docs.couchbase.com/server/current/learn/clusters-and-availability/connectivity.html#alternate-addresses]

//footnote[rest-set-up-alternate-address][https://docs.couchbase.com/server/current/rest-api/rest-set-up-alternate-address.html]


//footnote[couchbase-cli-setting-alternate-address][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli-setting-alternate-address.html]

