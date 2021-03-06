
= アーキテクチャー

== 分散アーキテクチャー

分散アーキテクチャーでは、複数のサーバーを用いて、ひとつのプラットフォームを構成します。これを@<strong>{クラスター}と呼びます。また、クラスターを構成する各サーバーは@<strong>{ノード}と呼ばれます。
クラスターは、ノード(物理サーバー、仮想マシン、あるいはコンテナ)の追加により、CPU、RAM、ディスク、およびネットワークの容量を増やすことができます。

Couchbase Serverは、シェアードナッシング型の@<strong>{分散アーキテクチャー}を特徴としており、拡張性と可用性に優れています。

=== ピアツーピアトポロジー

Couchbase Serverにおいては、他の分散システムに見られるようなマスターノード、スレーブノード、構成ノード、ネームノード、管理(ヘッド)ノードなどの概念はなく、@<strong>{全てのノードにおいてソフトウェアは同一}です。
全てのノードは同等のレイヤーで相互に通信します（@<strong>{ピアツーピアトポロジー}）。
Couchbase Serverは通常、複数のコモディティサーバーから構成されるクラスターとして構成されますが、開発などの目的で、全ての機能を単一のノードで実行することもできます。
その場合であっても、他の分散システムに見られるような(たとえばスタンドアローンモードのような)複数ノードで構成した場合とは異なる、特別なコンポーネント配置が用いられる訳ではありません。
クラスターを単一のノードで構成した場合も、複数のノードで構成した場合も、論理的なアーキテクチャーは変わりません。

Couchbase Serverでは、ノードがクラスターの一部として構成される(クラスターに参加する)際に、そのノードの特定のサービスを有効または無効にできます(Couchbase Server管理者は、クラスターへノードを追加する際に、そのノードで実行されるサービスを選択します)。
ただし、一旦そのノードがクラスターに参加した後は、そのノードで動いているサービスを変更することはできません。そのようなことを行いたい場合には、一度そのノードをクラスターのメンバーから削除し、改めて参加させる際にサービスを選択し直します。

=== ピアツーピアレプリケーション

Couchbase Serverはアクティブなデータの複製(レプリカ)を自動的に作成し、それらのレプリカをクラスター内のノードに分散して、全てのコピーが別々のノードに配置されるようにします。
これはたとえば、プライマリサーバーとセカンダリサーバーのように、ノードのレベルで異なる役割を持つアーキテクチャーとは異なっています。
Couchbase Serverのこのような特徴は、@<strong>{ピアツーピアレプリケーション}と呼ばれます。Couchbase Serverは、最大3つのレプリカをサポートします。

ピアツーピアレプリケーションは、アクティブデータのコピーを他のノードに複製するため、単一障害点を排除します。
ノードがダウンした場合、クラスター内の他の場所に存在するレプリカをアクティブ化することによって、そのデータを回復します。このプロセスはフェイルオーバーと呼ばれます。フェイルオーバーは自動または手動で行うことができます。

レプリケーションが提供する冗長性は、ノード障害からデータを保護し、可用性を向上させます。
Couchbase Serverでは、レプリカは高可用性のみを目的としており、アクティブデータへの昇格が行われない限り、通常は使用されません。
これは、分散されたデータを複数の計算機での分散(並列)処理のために用いる分散システムとは異なっています。
そのようなアーキテクチャーで発生する、データの一貫性を維持するための処理に伴うオーバーヘッドは、Couchbase Serverでは取り除かれています。
さらに、後で紹介するスマートクライアントとの組み合わせにより、高いスループットと低遅延が実現されています。@<fn>{performance-note}


//footnote[performance-note][1ノード当たりの性能として1秒間に数十万のリクエストを処理し、かつ、リクエストの99パーセントで1ミリ秒未満の応答時間が測定されることも珍しくありません。]

一方、レプリカを取得するためのAPIも提供されており、アクティブデータが利用できなくなってからフェイルオーバーが実行されるまでの期間、このAPIを用いることで、データ読み取りの可用性を向上させることができます。

//image[replica-concept][ピアツーピアレプリケーション]{
//}

(画像は、 Couchbase Under the Hood: An Architectural Overview@<fn>{server-arc-overview}より引用)

====[column]レプリカの数え方

分散アーキテクチャーを持つソフトウェアの中には、たとえばレプリカ数が3である(あるいは3レプリケーション)といった場合、3つのノードにデータが存在することを意味するものがあります。
Couchbase Serverでは、レプリカ数が3である場合、アクティブデータを含めて、4つのノードにデータが存在していることになります。
この「レプリカ」の意味の違いは、前者の種類のソフトウェアでは、複製されたデータの間に区別がないのに対して、Couchbase Serverはアクティブデータとレプリカデータが判然と区別されていることから生じています。

====[/column]


=== マルチディメンショナルスケーリング(MDS)

ノードはドキュメントストレージ（Dataサービス）、クエリ（Queryサービス）、インデックス（Indexサービス）等、特定のワークロードの処理専用に構成することができます。これによってワークロードを分離し、各サービスの要件と特性に応じたリソース配分にてクラスターを構成することができます。
他の分散アーキテクチャーを持つソフトウェアでは、ノードは全て同一のリソースを持つハードウェアで構成されることが必要ないし前提とされているのがしばしば見られます。それに対して、Couchbase Serverはリソース配分の柔軟性を特徴としており、各サービスに異なるノード数を割り当てられるだけでなく、各ノードでワークロード/サービスの特性に応じた異なるリソース(メモリー、CPU)を用いることができます。
こうした水平および垂直方向に拡張可能な柔軟なスケーリングは、@<strong>{マルチディメンショナルスケーリング(MDS)}と呼ばれています。

//image[mds-horizontal][クラスター構成例]{
//}

//image[mds-vertical][水平および垂直方向へのスケーリング]{
//}

(画像は、 Couchbase Under the Hood: An Architectural Overview@<fn>{server-arc-overview}より引用)

====[column]エディションによる差異

MDSは、エンタープライズエディションでのみサポートされています。具体的には、コミュニティーエディションでは、ノード上のサービスとしてDataサービスを無効にすることができません。つまり、エンタープライズエディションでは、Queryサービス等を専用のノードを用いて運用することができますが、コミュニティーエディションでは、Dataサービス以外のサービスは、常にDataサービスと同じハードウェアリソースを共有することになります。

====[/column]

=== オンラインリバランス

ノードの追加と削除は、データを均等に再分散するリバランスプロセスを通じて実現されます。
リバランスプロセスはオンラインで行われ、クライアント/アプリケーションはリバランス中もCouchbase Serverへ継続してアクセスすることができます。

クラスターは、リバランス中もリクエストを処理し続けます。そのため、リバランスのためのリソース消費は、クラスターの処理実行に影響を与えます。
Couchbase Serverでは、リバランスにおけるデータ移動の並列実行数の設定を通じて、リバランスに要する時間とその間のリソース消費とのバランスを最適化することができます。@<fn>{limiting-concurrent-vbucket-moves}

//footnote[limiting-concurrent-vbucket-moves][https://docs.couchbase.com/server/current/learn/clusters-and-availability/rebalance.html#limiting-concurrent-vbucket-moves]

=== データベースチェンジプロトコル(DCP)

Couchbase Serverの各ノードは、データの複製、インデックスの更新などのために相互に通信します。
その際、Couchbase Serverは、固有のストリーミングプロトコルである@<strong>{データベースチェンジプロトコル(DCP)}を用います。DCPは、Couchbase Serverで生じたデータの変更をクラスター内部に伝える役割を持ちます。
また、DCPはSpark、Kafka、Elasticsearchのような外部システムとの統合のために利用することができるコネクターへデータをフィードするためにも用いられます。

DCPにおけるデータ転送は、メモリーのレイヤーで実行されます。つまり、Dataサービスでデータの変更が(メモリーレベルで)行われた後、ディスクへの反映を待たずに、DCPを介して他のノードへ転送されます。

DCPは、一時的なエラーに対して堅牢で回復力を持ちます。たとえばストリームが中断された場合、接続が再開されると、DCPは最後に正常に更新された時点から再開します。

=== vBucket

Couchbase Serverは内部的に、@<strong>{vBucket}を使用して、データを管理しています（クライアント/アプリケーションはvBucketを直接操作しません）。
シャードまたはパーティションのようなものといえば、これらの用語に慣れている方はにとっては理解しやすいかもしれません。あるいは、もっと直接的に、OS上の物理ファイルに対応しているともいえます。

ひとつのバケットは1024個のアクティブvBucketと、（１レプリケーション毎に）1024個のレプリカvBucketに分割して管理されています。この数はクラスターを構成するノードの数に左右されません。
vBucketは、クラスター内でDataサービスを実行しているノード全体に均等に分散されます。Dataサービスを実行しているノードの数に増減が生じた場合には、vBucketは再分配されます。


== スマートクライアント

=== クラスターマップ

Couchbase Serverのクライアントは、接続しているクラスターのトポロジー(クラスターの構成)を認識し、トポロジーに変更が発生しても、透過的に最新の状態を認識します。
これは、@<strong>{クラスターマップ}と呼ばれる内部情報が、クラスターからクライアントへ提供されることによって実現されます。

クライアントは、クラスターへの初回接続（ブートストラップ）時にクラスターマップを入手し、クライアントとクラスターとが接続されている間、クラスターマップのローカルコピーは自動的に最新の状態に維持されます。


クライアントはDataサービスを利用する際、このクラスターマップによって実現されるトポロジー認識に基づいて、目的のドキュメントを管理しているノードに直接アクセスします。

Couchbase Serverのドキュメントは、キースペースにおいて、一意のキーにより識別されます。キーは内部的に、CRC32ハッシュアルゴリズムを用いて変換されます。
このアルゴリズムにより、バケットを構成するvBucket中のドキュメントの分布は、ユーザーの付けたキーの内容に関わらず、均等になります。

Couchbase Serverクライアントは、ドキュメントのアクティブなコピーを見つける(ないし、新しく作成する)ために、ドキュメントキーに対して、CRC32ハッシュアルゴリズムを使用して、そのドキュメントが含まれる(ことになる)vBucketを識別します。
次に、クライアントはクラスターマップを参照して、現在どのノードにそのvBucketが含まれているかを判別します。

同様に、クラスターマップではQueryサービスを実行しているノードのリストも、最新の状態に維持されます。Queryサービスはステートレスであるため、任意のノードで任意のリクエストを処理できます。

//image[crc32hash][スマートクライアント]{
//}

(画像は、 Moving from Relational to NoSQL: How to Get Started@<fn>{relational-no-sql-wp}より引用)

//blankline

//footnote[relational-no-sql-wp][https://resources.couchbase.com/c/relational-no-sql-wp?x=Y7B0ca]

クラスターマップによるトポロジーの認識は、他の分散アーキテクチャーを持つデータプラットフォームには見られない、Couchbase Server独自の特徴です。
多くの分散アーキテクチャーでは、クライアントとのコミュニケーションを担当する特別なノードが存在し、データが保存されているノードとクライアントとの間を仲介する役割を果たします。
この違いは、Couchbase Serverがクライアント/アプリケーションに対して低遅延かつハイスループットな処理を提供することができる要因のひとつとなっています。


=== ブートストラップ

クライアントからCouchbase Serverへの初回接続は、認証と認可、検出(Discovery)、およびサービス接続の3つのフェーズで確立されます。この一連の過程は、@<strong>{ブートストラップ}と呼ばれます。

以下に、各プロセスの概要を記します。

//blankline

 1. @<strong>{認証と認可} クライアントは、ユーザー名とパスワードで認証されます。認証を介して、ユーザーはロールを取得します。ロールに含まれる権限によって、Couchbase Serverのリソースに対する操作が認可されます。
 2. @<strong>{検出(Discovery)} クラスターマップがクライアントに返されます。クラスターマップは、クラスターを構成するノードのリスト、ノード間のサービスおよびデータ配置に関する情報を含んでいます。クライアントはこのクラスターマップを使って、利用するサービスとデータに応じた接続先を検出することができます。
 3. @<strong>{サービス接続} クライアントは、サービスレベルの操作を実行するために必要な接続を確立します。クライアントが要求しているリソースへの操作に対する適切な権限に関連付けられているロールを持っている場合、アクセスが許可されます。

//blankline

なお、ブートストラップ時の接続先は、高可用性を実現するためにクラスターを構成する複数のノードのIPアドレスを用いて構成するのが一般的です。

== XDCR(Xross Data Center Replication)

====[column]エディションによる差異

コミュニティーエディションでは、XDCRは利用できません。

この制限は、バージョン7.0より導入されました(バージョン6.6までは、一部機能を除き利用できていました)。

エンタープライズエディションでしか利用できない機能は、通常コミュニティーエディションとして配布されているバイナリでは、機能自体が含まれていません(実行不可能である)。それに対して、XDCR機能は(少なくとも7.0では)表面上、利用可能な状態となっており、規約の上で、利用が制限されています。

====[/column]

=== XDCRは、なぜ必要か

XDCRは、Couchbase Serverクラスター間でデータを複製するために使用されるテクノロジーです。
XDCRを使用すると、ベアメタル、VM、プライベートクラウド、パブリッククラウド、コンテナなどのプラットフォームに関係なく、任意のクラスター間でデータを複製できます。
また、片方向のデータ同期のみではなく、双方向のデータ同期をサポートしています。

=== XDCRは、どのように動作するか

XDCRは非同期レプリケーションによる結果整合性を介して、サイト間でデータの一貫性を維持します。

XDCRは、クラスターの構成変更に対して透過的に動作します。ノードの追加ないし削除中であっても、継続して動作し続けます。

Couchbase Serverは、データベースチェンジプロトコル(DCP)と呼ばれるストリーミングプロトコルを持ち、順序付けられたキューを使用してデータの変更を通信します。XDCRはコンシューマーとして、DCPを利用しています。

//image[XDCR][XDCR]{
//}

(画像は、 Couchbase Under the Hood: An Architectural Overview@<fn>{server-arc-overview}より引用)

=== チェックポイント

XDCRはレプリケーション実行中、一定の間隔でチェックポイントを更新します。
レプリケーション中断後の再開時(これには、ユーザーによる計画的な実行、または障害からの回復のケースの両方が考えられます)、XDCRは最後のチェックポイントからレプリケーションを再開します。

チェックポイント更新の間隔が短いほど、レプリケーションの再開に必要な作業量が少なくなります。一方、頻繁なチェックポイントの更新が必要となり、更新のためのシステムリソース消費が、クラスターの性能に影響を与える可能性があります。

=== レプリケーション指定とフィルタリング

レプリケーションは、バケット単位で設定する他、スコープやコレクションを用いて制御することができます。

また、レプリケーションの対象とするデータを、データの内容などに応じてフィルタリングするための方法（レプリケーションフィルター）が用意されています。

====[column]エディションによる差異

コミュニティーエディションでは、(XDCRが利用できるバージョンであっても)フィルタリング機能は利用できません。

====[/column]


=== レプリケーション優先度

あるバケットのレプリケーションが既に実行されている環境で、新しく別のバケットのために新規レプリケーションを追加するようなケースでは、新規レプリケーションの実行が、既存のレプリケーションのスループットに悪影響を与える場合が考えられます。たとえば、既存のレプリケーションでは、変更されたデータの更新のみが行われています。それに対して、新規にレプリケーションが追加されたバケットに既にデータが格納されている場合、全てのデータのレプリケーションが一斉に実行されることになります。
この場合のように、全てのレプリケーションに同じ優先度が適用されることが不適切なケースがあります。

XDCRでは、レプリケーション毎に優先度を設定することができ、優先度に基づいた適切なリソース配分が可能です。

=== ネットワーク帯域幅節約のための最適化

XDCRは、データ圧縮のような内部機構によって、ネットワークの帯域幅を節約するために最適化されています。
また、ユーザーによる設定（たとえば、高度な設定@<fn>{xdcr-advanced-settings}として提供されている「オプティミスティックレプリケーション」）を介して、環境や用途に応じた最適化を行うことが可能です。

=== 運用上の注意

XDCR処理は、Dataサービスが稼働しているノードで実行されるため、多かれ少なかれDataサービスに対する性能影響があります。

適切にリソース見積もりが行われることが重要ですが、たとえば、運用中のクラスターで、新たにXDCRを利用する要件が生まれた際には注意が必要です。典型的なケースとして、データセンターのマイグレーションが考えられます。状況に応じて、マイグレーションのためにデータのレプリケーションを行う期間、一時的にノードを追加し、クラスターを拡張するといった対応が考えられます。

== サーバーグループ

XDCRが、データセンターレベルで(複数のクラスターを用いて)可用性を高めるために利用できるのに対して、データセンター内の物理配置レベルで(ひとつのクラスターを用いながら)可用性を高めるための仕組みとして、サーバーグループが存在します。

データセンター(ネットワーク)内の物理的な区画(ラックやアベイラビリティゾーン等)に合わせて、複数のグループを定義して、サーバーの物理配置に合わせて、ノードをそれぞれのグループに割り当てることができます。

ここでは、ふたつの異なる区画があり、クラスター内で複製して管理することができる対象(vBucketとインデックス)について、アクティブデータとそのレプリカがひとつあると考えてみます。
この場合、ふたつのグループを定義し、物理配置に合わせてノードをそれぞれのグループに割り当てます。
サーバーグループの定義によって、アクティブデータとレプリカとは、別々のグループで維持されます(サーバーグループが定義されていない場合は、配置はクラスター内でランダムに行われます)。
そのため、片方の区画の物理的なダメージにより、一方のグループがオフラインになっても、もう一方のグループでデータの完全なセットが維持されます。


====[column]エディションによる差異

コミュニティーエディションでは、サーバーグループは利用できません。

====[/column]

//footnote[server-arc-overview][https://resources.couchbase.com/c/server-arc-overview?x=V3nd_e]

//footnote[xdcr-advanced-settings][https://docs.couchbase.com/server/current/xdcr-reference/xdcr-advanced-settings.html]

