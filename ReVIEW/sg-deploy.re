
= Sync Gatewayシステム設計


Sync Gatewayをシステム構築に用いる際における、設計上の留意事項について解説します。

//footnote[sync-gateway-deployment][https://docs.couchbase.com/sync-gateway/current/deployment.html]

Sync Gatewayを介したCouchbase LiteとCouchbase Serverとのデータ同期を行うに際して、Sync Gatewayエンティティに由来する観点に加えて、Couchbase Serverの技術仕様に応じた考慮も必要になります。

また、Sync Gatewayのデプロイメント(Deployment@<fn>{sync-gateway-deployment})における、ハードウェアやOSに関する留意事項についても解説します。

== エンティティ設計

ユーザーやチャネルのような、Sync Gatewayで用いられるエンティティの数は、プロジェクトの要件によって異なります。

エンティティ情報は、ディスクやメモリのスペースを消費するため、要件に応じて、それらに関わるデフォルト値を変更したり、名前設計を考慮する必要が生じる場合があります。


=== チャネル

チャネルは、ユーザーとドキュメントの両方と関係を持ちます。

Sync Gatewayで用いることのできるチャネル数については、固定された上限値はありません。
ただし、Sync Gateway内部における情報の保存方法に関係して実際上の上限が存在します。

以下、Sync Gateway内部のチャネル情報管理について、ユーザーとドキュメントそれぞれの観点から解説します。
また、設計上の具体的な指針についても示します。

//blankline

はじめに、@<em>{ユーザー当たりのチャネル数}について、見ていきます。

ユーザーに割り当てられているチャネルの情報は、Sync Gatewayユーザー情報の一部として管理されます。
Sync Gatewayユーザー情報は、Couchbase Serverのドキュメントとして保存されます。

そのため、１ユーザーに対して割り当てることのできるチャネルの数は、チェネルによって消費されるデータ量による限界を持ちます。
ユーザー情報は、Couchbase Serverドキュメントの最大サイズである20MB以内に収まる必要があります。
通常1ユーザーあたり1000チャネル程度の運用が可能とされています。

//blankline

次に検討するのは、@<em>{ドキュメント当たりのチャネル数}です。

ドキュメントが新しいチャネルに割り当てられるたびに、チャネル名がそのドキュメントのメタデータに追加されます。
そのため、チャネルへのアクセス許可情報によって消費されるデータのサイズが、ドキュメントの拡張属性(XATTR)の最大サイズである1MB内に収まる必要があります。

したがって、ドキュメントを複数のチャネルに割り当てる場合、割り当てるチャネルの最大数には実際上の上限があります。
Sync Gatewayは、許可された制限を下回っている限り、ドキュメントを新しいチャネルに割り当てます。

通常1ドキュメントあたり50チャネル程度の運用が可能とされています。

//blankline

ユーザーやドキュメントあたりのチャネル数が使用可能なスペースを超えることが見込まれる場合、次の事を検討します。

//blankline

 * チャネル名を短くします。チャネル名が短いほど、占有するスペースが少なくなります。
 * 必ずしも個別に持つ必要がないチャネルを統合するなど、システムで利用するチャネルの数を減らせないかどうかを検討します。
 * ドキュメントあたりのチャネル数を減らすために、ドキュメントのリビジョンの数を制限します。チャネル情報はリビジョンごとに保持されます。

=== ユーザー/ロール

Sync Gatewayのユーザーやロールの情報は、Couchbase Serverのバケットに格納されます。
そのため、ユーザーやロールの数は、バケットの容量を消費することになります。
特に、コンシューマー向けモバイルアプリの様に、サービスの利用規模拡大にあわせて、ユーザー数が増加する様なユースケースでは、注意が必要になります。

== ドキュメント設計

Sync Gatewayを使用して同期するドキュメントのデータモデリング(Data Modelling@<fn>{data-modeling})について解説します。
Couchbase Liteを単体で利用する場合と異なり、データ同期を行う場合に固有の考慮事項があります。


//footnote[data-modeling][https://docs.couchbase.com/sync-gateway/current/data-modeling.html]

=== ドキュメントサイズ

Couchbase Serverのドキュメントの最大サイズは20MBです。
Couchbase Liteには、ドキュメントの最大サイズ制約はありませんが、Couchbase Serverと同期される場合、Couchbase Liteのドキュメントもこの制限に従う必要があります。


=== ユーザー定義プロパティ

Sync Gatewayには、ドキュメントのプロパティに使われる予約済みのキーワードが存在します。
予約済みのキーワードは、@<tt>{_id} 、@<tt>{_rev}、@<tt>{_sequence}のようにアンダースコア(@<tt>{_})を接頭辞として持ちます。
先頭にアンダースコアが付いたユーザー定義のプロパティがドキュメントのトップレベルに含まれる場合、Sync Gatewayによって同期を拒否されます。
その場合、次のようなエラーが発生します。


//emlist{
"{"error":"Bad Request","reason":"user defined top level properties beginning
 with '_' are not allowed in document body"}"
//}

Couchbase Liteは、このようなプロパティ名の使用を明示的にエラーとはしません。
これは、ドキュメントをCouchbase Serverと同期せずにローカルのみで使用する場合には制約とならないためです。
Couchbase Serverとの同期を行う場合は、Couchbase Liteでアンダースコア文字から始まるプロパティ名を使用することを避ける必要があります。


このルールは、共有バケットアクセスが有効になっている場合には、Couchbase Liteデータベースのみではなく、Couchbase ServerデータベースとCouchbase Serverクライアントアプリケーションの設計にも適用される必要があります。

このように、Sync Gatewayは内部管理情報をドキュメントやその拡張属性(XATTR)に保持します。
ドキュメントの同期のために使用されるこうしたメタデータはSync Gatewayによって内部的に管理されており、その構造は予告なく変更され得ます。
Sync Gatewayのメタデータの内容をアプリケーションロジックのために使用することはできません。

=== 添付ファイル

Couchbase Liteの添付ファイルは、Couchbase Serverに同期される際に、添付先ドキュメントと同じバケットに別のドキュメントとして保存されます。
同じ添付ファイルが複数のドキュメントで共有されている場合、添付ファイルのひとつのインスタンスのみが保存されます。

Couchbase Serverとのレプリケーションを利用する場合、以下に留意する必要があります。

//blankline

 * 添付ファイルのサイズは、ドキュメントの最大サイズである20MB以内である必要があります。
 * Couchbase Liteでは、ドキュメントに関連付けることができる添付ファイルの数に固定の上限はありません。ただし、添付ファイルの属性情報がドキュメントに保存されるため、添付ファイルの数は、ドキュメントとして保存可能なサイズの上限によって、実際上制限されます。

//blankline


なお、添付先ドキュメントが更新されていても、添付ファイル自体に変更がない場合には、添付ファイルは同期の対象とされません。

=== リビジョン

リビジョンの情報は、ドキュメント内部に格納されます。

Sync Gatewayでは、@<tt>{revs_limit}設定によって、保持されるリビジョンの数を変更することができます。
Sync Gatewayの@<tt>{revs_limit}のデフォルト値は1000であり、許容される最小値は20となっています。

なお、Couchbase Liteでは、最大20のリビジョンを保持します。最大値を変更することはできません。


== 性能設計

=== クラスター

Sync Gatewayは、複数のSync Gatewayノードからなるクラスターとして構成することによって、水平方向にスケーリングすることが可能です。

単一のSync Gatewayノードがサポート可能な同時ユーザー接続数は、そのノードが利用することのできるハードウェアリソースによって決まります。
そのため、垂直方向のスケーリングには、自ずから限界が存在します。

垂直方向のスケーリング(スケールアップ)と、水平方向のスケーリング(スケールアウト)のそれぞれの特色に応じて、クラスターの構成を検討することが重要です。

=== ノード/サーバー


Sync Gatewayの設計仕様との関係におけるノード/サーバーレベルの性能設計に影響する論点を以下に整理します。

//blankline

 * Sync Gatewayは、ブートストラップ構成ファイル以外のローカル状態を保持しないため、ディスクの性能や容量については考慮する必要がありません。
 * Sync Gatewayは、RAMにチャネルとリビジョンのメタデータキャッシュを維持します。そのためRAMのサイズについて考慮する必要があります。
 * Sync Gatewayは、マルチプロセッシング用に設計されており、軽量スレッドと非同期I/Oを使用します。そのため、Sync Gatewayは、CPUコア数に応じてパフォーマンスを上げることができます。
 * Sync Gatewayでは、すべての書き込み操作はSync関数によって処理され、読み取りアクセス権を持つ他のクライアントへの通知をトリガーします。そのため、書き込み操作は読み取り操作よりもシステムに大きな負荷をかけます。
 * 継続的レプリケーションを実行している各クライアントは、オープンソケットを維持します。これらのソケットはほとんどの時間アイドル状態であるため、オープンソケットの数について留意する必要があります。OSレベルチューニングを検討する場面がありえます(後の部分で解説します)。
 * Sync GatewayはRAM利用に最適化されているため、Linuxのswappiness値を0に変更することでパフォーマンスを向上させることができます。これにより、少なくとも性能が劣化することはありません。詳しくはドキュメント(Swap Space and Kernel Swappiness@<fn>{install-swap-space})を参照ください。


//footnote[install-swap-space][https://docs.couchbase.com/server/current/install/install-swap-space.html]


====[column]Sync Gatewayキャッシュ設定

Sync Gatewayエンタープライズエディションでは、内部で利用されるキャッシュを調整するオプションが提供されています。

このキャッシュ設定は、チャネルとリビジョンに対して構成可能です。詳細はドキュメント(Channel and Revision Cache@<fn>{channel-and-revision-caches})を参照ください。

なお、コミュニティエディションでは、これらのプロパティが設定されていても無視され、デフォルト値が適用されます。

====[/column]

//footnote[channel-and-revision-caches][https://docs.couchbase.com/sync-gateway/current/deployment.html#channel-and-revision-caches]

== OSレベルチューニング

Sync GatewayをデプロイするにあたってのOSレベルチューニング(OS Level Tuning@<fn>{os-level-tuning})について解説します。



//footnote[os-level-tuning][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html]

=== ファイルディスクリプタ設定


Sync Gatewayプロセスが使用可能なファイルディスクリプタ@<fn>{wiki-File_descriptor}の最大数を増やすことは、Sync Gatewayがオープンすることができるソケットの最大数、つまりSync Gatewayが同時にサポートできるクライアントの最大数に直接影響します。
これには、OSのパラメーターを調整する必要があります。

//footnote[wiki-File_descriptor][https://en.wikipedia.org/wiki/File_descriptor]


ここでは具体的な内容については説明しませんが、OSのファイルディスクリプター制限の操作(Operating System File Descriptor Limits@<fn>{operating-system-file-descriptor-limits})について、Sync Gatewayドキュメントで解説されています。
Sync Gatewayをサービスとして管理している場合には、サービスのファイルディスクリプタ制限(Service File Descriptor Limits@<fn>{service-file-descriptor-limits})を変更します。
サービスを利用せずにSync Gatewayを運用している場合には、プロセスのファイルディスクリプタ制限(Process File Descriptor Limits@<fn>{process-file-descriptor-limits})を変更します。

//footnote[operating-system-file-descriptor-limits][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html#operating-system-file-descriptor-limits]
//footnote[sync-gateway-file-descriptor-limits][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html#sync-gateway-file-descriptor-limits]
//footnote[service-file-descriptor-limits][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html#service-file-descriptor-limits]

//footnote[process-file-descriptor-limits][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html#process-file-descriptor-limits]


 OSの設定にあわせて、Sync Gateway構成上のファイルディスクリプタ制限(Sync Gateway File Descriptor Limits@<fn>{sync-gateway-file-descriptor-limits})を構成します。

=== TCPキープアライブ設定


Sync Gatewayで使用可能なファイルディスクリプタの最大数を増やしても、「too many open files」というエラーが発生する場合があります。
この場合、TCPキープアライブ設定を調整する必要があります。


モバイルアプリケーション利用時においては、クライアントが接続を閉じることなく、ネットワークから突然切断される場合がありえます。
これらの接続はSync Gatewayによって、デフォルトでは約7200秒(2時間)後にクリーンアップされますが、このような状態が蓄積されることにより、「too many open files」というエラーの発生に繋がります。

エラーが発生した場合に、次のコマンドを使用して、Sync Gatewayとの間に確立された接続の数を数えることができます。


//cmd{
$ lsof -p <Sync GatewayのプロセスID> | grep -i established | wc -l
//}


上記の方法で確認した値がファイルディスクリプタの最大制限付近である場合、TCPキープアライブパラメータを調整して、デッドピアがソケットを開いたままにする時間を短縮することが考えられます。


ただし、TCPキープアライブ設定を用いた調整には欠点がないわけではなく、TCP/IPスタックが頻繁にキープアライブパケットを送信するため、システム全体のネットワークトラフィックの量が増加することに注意が必要です。

CentOSでのTCPキープアライブ設定手順について、Sync Gatewayドキュメント@<fn>{_linux_instructions_centos_1}で解説されています。

//footnote[_linux_instructions_centos_1][https://docs.couchbase.com/sync-gateway/current/os-level-tuning.html#_linux_instructions_centos_1]



