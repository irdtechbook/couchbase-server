
= 環境設定

本章は、Couchbase Serverの環境設定に関する網羅的な解説、あるいは実際に設定を行うために必要な手順の説明を意図していません。

アプリケーションの要件やシステム環境に応じて、意思決定が必要な設定の紹介を通じて、Couchbase Serverに特有の概念や機能に関して理解を深めていただくことを目的としています。

網羅的な設定項目の解説については、適宜ドキュメント@<fn>{manage-settings}を参照してください。

//footnote[manage-settings][https://docs.couchbase.com/server/current/manage/manage-settings/manage-settings.html]

== メモリーからのデータ排出(Ejection)


=== 概要

Couchbase Serverのバケットにはメモリークォータが設定され、専用のメモリー領域が確保されます。
Couchbase Serverはこのメモリー領域を用いて、可能な限りすべてのデータをメモリー上に保持しようとします。

メモリー内のデータがバケットのメモリークォータの85％（ハイウォーターマークと呼ばれます）に達した場合、データがメモリーから排出(削除)され始めます。この際、最近使用されていないデータから優先的に排出されます。
この排出プロセスは、メモリークォータの75％（ローウォーターマークと呼ばれます）を下回るまで続きます。


//image[ejection_conceptual_diagram][バケットのメモリークォータとハイ/ローウォーターマーク]{
//}

(画像は、Couchbase Serverドキュメント Ejection@<fn>{ejection}より引用)

//blankline

//footnote[ejection][https://docs.couchbase.com/server/current/learn/buckets-memory-and-storage/memory.html#ejection]

=== 排出方法オプション

メモリーからのデータ排出時のオプションとして、次のいずれかの方法を選択します。@<fn>{a-tale-of-two-ejection-methods-value-only-vs-full}

//blankline

 * @<strong>{Value-only}: 値のみが排出され、キーとメタデータはメモリーに残ります。より多くのメモリーが必要ですが、最高の性能が提供されます。
 * @<strong>{Full}: （キー、メタデータ、値を含む）すべてが排出されます。メモリー要件を軽減します。



//footnote[a-tale-of-two-ejection-methods-value-only-vs-full][ブログ記事「A Tale of Two Ejection Methods: Value-only vs. Full」(https://blog.couchbase.com/a-tale-of-two-ejection-methods-value-only-vs-full/)にて、オプションの選択が、性能とメモリー消費に与える影響が詳細に解説されています。]

=== 選択基準

排出方法の選択基準として、以下のように整理することができます。

//blankline

 * @<strong>{ドキュメント数が一定または予測可能}: 事前の計画に基づいたリソース設計により、@<b>{Value-only}を選択することができ、最高の性能を得ることができます。
 * @<strong>{ドキュメント数の増加ペースの予測が難しい}: キーやメタデータは、それ自体のサイズは決して大きくありませんが、ドキュメント数に応じて増えるため、全てがメモリーに収まりきれなくなる事態が起こりえます。この場合、@<b>{Full}を選択することで、メモリー容量超過の問題を回避することができます。

=== 設定変更時の注意点

排出方法の設定は、バケット作成時に行えるだけではなく、作成後の変更も可能です。
ただし、この変更の反映には、（設定変更時に暗黙に行われる）バケットのリスタートが伴うことに注意が必要です。
そのため、アプリケーション実行中に設定変更を行った場合、アプリケーションからの接続がリセットされる事態が発生する可能性があります。

=== メモリー消費状況の把握

上述のように、排出方法の選択は、クラスターの性能と大きな関係があります。その意味で、安全性を重視して安易に@<b>{Full}を選択するのではなく、@<b>{Value-only}を選択して、高い性能を実現したいと考えるのは自然です。

また、事前の計画に基づいて、@<b>{Value-only}を選択した場合であっても、計画外の事態が発生する可能性を考慮することは依然として重要です。

ここでは、上記のような課題に対して利用できる機能を紹介します。

//blankline

@<strong>{監視} 

Webコンソールでは、下記のように、RAMに占めるデータおよびメタデータに関する情報を確認することができます。

ここでは機能を紹介するためにダッシュボード画面を示しましたが、Couchbase Serverでは、ダッシュボードと同内容の監視項目をコマンドラインやREST APIを用いて利用することができます。そのため、これらを外部監視システムとの連携のために利用することが考えられます。

//image[data_monitoring][RAM利用状況グラフ]{
//}

//blankline

@<strong>{警告} 

また、Couchbase Serverには特定の監視項目について警告を発生させる機能があり、メタデータオーバーヘッドに関する警告を利用することができます。

下のような、Webコンソール上のポップアップの他、Eメールを送信することができます。

//image[data_alert][メタデータオーバーヘッドに関する警告]{
//}

== コンパクション

=== 概要

Couchbase Serverにおけるデータの挿入、更新、削除は全て、物理的にディスク上のデータファイルに対する「追加」書き込みとして実現されます。
その中でも、削除されたドキュメントの情報は、特に「トゥームストーン(Tombstone)」と呼ばれます。

この結果として生じる、物理データファイルと論理データとの(特にデータが削除された場合に顕著な)ギャップは、@<strong>{コンパクション}と呼ばれるプロセスを通じて定期的に解消されます。

コンパクションは、Couchbase Serverの実行中にバックグラウンドプロセスとして実行されます。
データベース操作をシャットダウン、または一時停止する必要はありません。

コンパクションが実行される際には、新しいファイルが作成されます。コンパクション実行中、既存のデータファイルはそのまま残り、情報の格納やインデックスデータの更新に引き続き使用されます。
このプロセスにより、コンパクションが行われている間もデータベースが引き続き使用可能になります。
コンパクションが完了すると、古いデータファイルが無効になります。同時に、新しく作成されたデータファイルを使った更新が開始されます。その後、古いデータファイルが削除されます。

Couchbase Serverではコンパクションは自動的に実行されます。

また、REST API@<fn>{rest-compact-post}や、コマンドラインツール(@<tt>{couchbase-cli})を用いて、コンパクションを手動で実行することも可能です。


=== 運用上の留意点

コンパクションの運用にあたり、下記に注意を払う必要があります。

//blankline

 * @<strong>{余裕のあるCPU利用状況}: コンパクションプロセスはCPUを集中的に使用します。

 * @<strong>{十分なディスク領域}: コンパクションは新しいファイルを作成します。そのため、コンパクションの実行には、現在のデータファイルの倍のディスク領域が必要になります。

//blankline

Couchbase Serverでは、コンパクションが実行される前にディスク領域がチェックされます。使用可能なディスクスペースが現在のデータファイルサイズの2倍未満の場合、コンパクション処理は実行されずにログに警告が出力されます。

=== オートコンパクション

オートコンパクション実行のトリガーとして、下記のふたつがあります。

//blankline

 * @<strong>{データファイルの断片化}: データファイルが断片化されている割合をパーセンテージで設定します。たとえば、10%に設定されている場合、データファイルが10%断片化された時点でオートコンパクションが実行されます。
 * @<strong>{時間帯}: オートコンパクションを許可する時間帯を設定して、それ以外の時間帯にオートコンパクションが行われないようにすることができます。デフォルトでは、コンパクションは順次実行されます。



//footnote[compaction-rest-api][https://docs.couchbase.com/server/current/rest-api/compaction-rest-api.html]

//footnote[rest-compact-post][https://docs.couchbase.com/server/current/rest-api/rest-compact-post.html]



== 代替アドレス

=== 概要

Couchbase Serverでは、クラスターを構成するノードに対して、プリンシパルアドレスに加えて、代替(Alternate)アドレス@<fn>{alternate-addresses}を割り当てることができます（また、代替ポート番号をそのノードで実行されているサービスに割り当てることもできます）。

プリンシパルアドレスは、クラスターの初期構成時に設定されるアドレスです。代替アドレスは必要に応じて追加します。

代替アドレスの設定はCLI@<fn>{couchbase-cli-setting-alternate-address}またはREST API@<fn>{rest-set-up-alternate-address}で実行可能です。

ここでは、代替アドレスが使われるふたつのパターンを紹介します。


=== Couchbase Serverの特殊性との関係

まず、代替アドレス設定について理解するための前提を整理します。

ひとつのサーバーに複数のアドレスが割り当てられることは珍しいことではありません。ですが、分散アーキテクチャーを持つクラスターにおいては、モノリシックなアーキテクチャであれば考慮する必要がない面を考慮する必要があります。

Couchbase Serverでは、クライアント/アプリケーションがクラスターに接続する際には、クラスターマップという要素が介在します。
初期接続は、特定のノード（複数指定可能）に対して行われます。接続確立（ブートストラップ）時に、接続したノードからクラスターマップを受け取り、それ以降のリクエストは、対象となるノードに対して直接行われます。

そのため、ブートストラップのためにクライアント/アプリケーションが明示的に利用している特定のアクセス先へのリクエストだけを考えて、IPやドメイン名の解決が外部的に整合性が取れていればいいわけではありません。クラスターを構成する全てのノードに関する内部情報(クラスターマップ)が代替アドレスと整合している必要があります。

=== 外部ネットワークからの接続

外部ネットワークからクラスターへ接続するケースについて、下記に図示します。

まず注意しておくと、この図ではアプリケーションが外部ネットワークにあり、内部ネットワークにあるCouchbase Serverを利用していますが、これは必ずしも典型的なケースとはいえません。
一般的には、アプリケーションとCouchbase Serverは同じ内部ネットワークにあり、その場合、アプリケーションとCouchbase Serverとの接続には、プライベートIPが使われればこと足ります。この場合、代替アドレスを設定する必要はありません。

外部ネットワークにあるアプリケーションとの接続に限らず、複数の異なるネットワークにあるクラスター間でXDCRを実行する場合も、このケースに該当します。

//image[nat][外部ネットワークからのクラスターへのアクセス]{
//}

(画像は、 Couchbase Serverドキュメント Alternate Addresses - Internal Network or Cloud Access@<fn>{internal-network-or-cloud-access}より引用)

//blankline

//footnote[internal-network-or-cloud-access][https://docs.couchbase.com/server/current/learn/clusters-and-availability/connectivity.html#internal-network-or-cloud-access]



このケースでは外部からの参照のために、ネットワークアドレス変換（NAT）機能で各サーバーにパブリックIPを付与し、それらを代替アドレスに割り当てます。

プリンシパルアドレスには、プライベートIPアドレスが利用されます。ここではクライアントからの接続に焦点を当てていますが、プリンシパルアドレスはクライアントからの接続のみではなく、クラスター内のノード間の接続にも利用されることは理解しておく価値があります。
このことから、プリンシパルアドレスに単に外部アドレスを割り当てることは適切ではない、ということがわかります。

=== デュアルネットワーク

次の図で示すように、セキュリティーを最適化するために、デュアルネットワークを使用して複数のアプリケーションのノードへのアクセスを分離することが考えられます。

//image[dual_network][デュアルネットワーク環境におけるクラスターへのアクセス]{
//}

(画像は、 Couchbase Serverドキュメント Alternate Addresses - Dual Network@<fn>{dual-network}より引用)

//blankline

//footnote[dual-network][https://docs.couchbase.com/server/current/learn/clusters-and-availability/connectivity.html#dual-network]

ここではふたつのサーバーがあり、それぞれプライマリアドレスおよびセカンダリアドレスを持っています。それぞれのアドレスをプリンシパルアドレスと代替アドレスに割り当てています。そして、ふたつのアプリケーションはそれぞれ割り当てられたプリンシパルアドレス、または代替アドレスのいずれかを利用します。



//footnote[alternate-addresses][https://docs.couchbase.com/server/current/learn/clusters-and-availability/connectivity.html#alternate-addresses]

//footnote[rest-set-up-alternate-address][https://docs.couchbase.com/server/current/rest-api/rest-set-up-alternate-address.html]


//footnote[couchbase-cli-setting-alternate-address][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli-setting-alternate-address.html]

