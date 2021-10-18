
= フェイルオーバー

== フェイルオーバー概観

Couchbase Serverにおけるフェイルオーバーの解説に進む前に、まず分散アーキテクチャーにおけるフェイルオーバーについて、概観します。

まず、広い意味でのフェイルオーバーとして、以下の２種類があります。

=== 計画的なノード削除（グレースフルフェイルオーバー）

クラスターから機能しているノードを削除するには、まずノードを削除対象としてマークします。
その後、クラスターをリバランスし、これまで、そのノードへ行われていたリクエストが他のノードで処理できるようにします。
削除対象ノードは、リバランスが完了するまでリクエストを処理します。リバランス完了後に、他のノードでリクエスト処理が開始されます。
このようにして、サービスが中断することも、データを消失してしまうこともなく、グレースフルに構成を縮小(Graceful degradation@<fn>{wiki-Graceful_degradation})することができます。

//footnote[wiki-Graceful_degradation][https://en.wikipedia.org/w/index.php?title=Graceful_degradation&redirect=no]

=== 障害時のノード削除（ハードフェイルオーバー）

ハードフェイルオーバーは計画的なノードの削除とは異なり、正常に動作していないノードに対して実行します。
ノード障害時に他のノードへ複製されていないデータが存在していた場合、データロスとなります。

//blankline

以下、上記のハードフェイルオーバーに関する論点を整理していきます。

=== フェイルオーバー実行方法と考慮点

ノード障害発生時に、クラスターを縮退稼働させるために、フェイルオーバーを手動で実行するか、あるいは自動的にフェイルオーバーがトリガーされるようにします。

//blankline

 * @<strong>{自動フェイルオーバー}を利用すると、ユーザーによる操作なしにフェイルオーバーできますが、障害を発生させた問題の調査や特定は難しくなります。
 * @<strong>{手動フェイルオーバー}によりクラスターを管理する場合、クラスターを監視し、問題の発生を検知できるようにします。監視や手動の操作が必要になります。


=== 自動フェイルオーバーにおける問題

障害の原因を特定できない場合、またはシステムにかかる負荷を理解していない場合、自動フェイルオーバーは、問題を発生させる可能性があります。
たとえば、次のような状況を考えてみることができます。

クラスターが、キャパシティの限界付近（たとえば、80%から90%）で稼働しているとします。
このとき、ノード障害が発生し、ソフトウェアが自動的にノードをフェイルオーバーした場合、残りのノードでは負荷が増大し、正常に処理を続けることはできないでしょう。
結果として、他のノードもダウンし、自動的にフェイルオーバーされてしまいかねません。
このように、障害が連鎖し、結果的にクラスター全体がダウンしてしまう可能性があります。

この場合に考えられるのは、ノード障害が発生してもクラスターを運用し続け、準備が整い次第、新しいサーバーをクラスターに追加し、キャパシティを補填した上で、ダウンしたノードを削除し、リバランスを実行することです。
こうすることによって、クラスター全体が利用不可能になる可能性を回避することができます。
この間、障害が発生しているノードに対して行われたリクエストは、失敗することになりますが、障害の連鎖により、全てのリクエストが処理できなくなる可能性を回避することができます。

もちろん、ノード障害発生時にも十分な余剰のキャパシティを確保し、縮退運転ができるようにしておくことが望ましいのは、いうまでもありません。

=== 自動フェイルオーバーにおける制約

分散アーキテクチャーを持つソフトウェアの多くは、以下の制約付きで自動フェイルオーバーを実装しています。

//blankline

 * 自動フェイルオーバーは、最低でも1クラスターに3台のノードを必要とする。これにより、ネットワーク分断が発生した際に、お互いのノードをフェイルオーバーしようとする状況が回避されます。これは、データの整合性と一貫性を保護するために重要です。
 * 自動フェイルオーバーは、データの複製が存在するなど、発動後もサービスが継続可能な場合にのみ実行される。
 * 自動フェイルオーバーは、(余剰キャパシティ見積の元、設定された)規定回数まで（典型的には一度だけ)発動する。これは、フェイルオーバーの連鎖による、性能や安定性の劣化を防ぎます。
 * 自動フェイルオーバーを実行するまでに一定の待機時間を設ける。これは一時的なネットワーク障害や、システムの遅延によって、誤ってノードがフェイルオーバーされることを防ぎます。

=== 手動フェイルオーバー

組織において、しかるべき意思決定者による判断を伴わずに、システムによって自動フェイルオーバーを実行することを許可していないことも珍しくありません。

手動ファイルオーバーによる運用であっても、REST APIやコマンドラインツールを使った、再現性のある操作を確立しておくことが重要であり、一般的です。

=== 外部システム連携

クラスターをモニタリングし、一定の発動条件に基づき、フェイルオーバーを実行するために外部システムを利用することも考えられます

外部システム利用には、ソフトウェアによる自動フェイルオーバーに比べて、クラスター以外の環境に関しても考慮できるという優位性があります。
たとえば、外部システムはクラスターが依存するネットワークスイッチがダウンしていることを検知するかもしれません。
このような場合、ノードをフェイルオーバーしても状況は改善しないことがわかるため、フェイルオーバーを実行しないという判断が可能になります。
あるいは、システム利用状況監視情報との併用により、クラスターを縮退させても残りのノードで集約したトラフィックを処理できると判断できた場合にのみ、フェイルオーバーを実行することも考えられます。



== 自動フェイルオーバー

Couchbase Serverにおける自動フェイルオーバー(Automatic Failover@<fn>{couchbase-automatic-failover})について、概略を整理します。

//footnote[couchbase-automatic-failover][https://docs.couchbase.com/server/current/learn/clusters-and-availability/automatic-failover.html]

=== 有効化設定

自動フェイルオーバーは、デフォルトでは無効となっています。
影響関係を理解した上で、明示的に有効とされるまで、自動フェイルオーバーは発生しません。

=== 待機時間

自動フェイルオーバーが発生するまでの待機時間を設定します。待機時間設定として許可される最短の値は30秒です。

環境に合わせて適切な待機時間を設定することは、サーバーやネットワークの一時的な問題により、機能しているノードをフェイルオーバーしてしまうことを防ぐために重要です。

=== 実行条件

自動フェイルオーバー実行には、いくつかの条件があります。これは自動フェイルオーバーが実行された際に発生する可能性のある問題を回避するためです。

下記のようなケースでは、自動フェイルオーバーが発生しません。

//blankline

 * @<strong>{イベント同時発生}: 複数のイベントが同時に発生した場合、自動フェイルオーバーはトリガーされません。
 * @<strong>{フェイルオーバー連続実行}: 管理者が指定した規定回数までしか、自動フェイルオーバーはトリガーされません。規定回数として許可される最大値は3です。自動フェイルオーバーの実行が規定回数に達すると、管理者がカウントを手動でリセットするまで、自動フェイルオーバーはトリガーされません。
 * @<strong>{データ損失発生可能性}: データ損失が発生する可能性のある状況では、自動フェイルオーバーはトリガーされません。たとえば、残りのノードにデータの複製がない場合はこのケースです。
 * @<strong>{非フェイルオーバーノードの応答状況}: フェイルオーバー対象となるノードを除いた後でなお、クラスターを構成するノードの過半数からの応答がある場合のみ、自動フェイルオーバーがトリガーされます。


=== サービス固有ポリシー

自動フェイルオーバーが適用されるための条件として、サービス固有のポリシーがあります。@<fn>{failover-policy}

Couchbase Serverでは、マルチディメンショナルスケーリング(MDS)により、ノード毎に配置するサービスを選択することが可能です。
フェイルオーバーの挙動は、そのノード上で稼働しているサービスにより決定されます。

//blankline

 * @<strong>{Dataサービス}では、最低３ノードが必要です。

 * @<strong>{Indexサービス}では、自動フェイルオーバーはサポートされません。

 * @<strong>{その他のサービス}（Query、Search、Analytics、Eventing）では、最低２ノードが必要です。


//footnote[failover-policy][https://docs.couchbase.com/server/current/learn/clusters-and-availability/automatic-failover.html#failover-policy]

====[column]エディションによる差異

コミュニティーエディションでは、マルチディメンショナルスケーリング(MDS)は利用できないため、Dataサービスのポリシーに準拠することになります。

====[/column]

=== 通知

自動フェイルオーバーが発生した際に、通知をメールで送信するように設定することができます。

=== グループフェイルオーバー

サーバーグループを定義してノードをグループ化している場合、グループフェイルオーバーの機能を活用することができます。@<fn>{defining-groups-and-enabling-group-failover}

グループフェイルオーバーは、デフォルトでは無効とされています。

グループに多数のノードが含まれている場合であっても、グループフェイルオーバーは単一のイベントと見なされます。
これにより、ラックの障害などのように、グループに含まれる複数のノードが同時に応答しなくなる事態が発生した場合に、「イベント同時発生(複数のイベントが同時に発生した場合、自動フェイルオーバーはトリガーされない)」の制約を回避することができます。

====[column]エディションによる差異

グループフェイルオーバーは、エンタープライズエディションでのみ使用できます。

====[/column]

//footnote[defining-groups-and-enabling-group-failover][https://docs.couchbase.com/server/current/learn/clusters-and-availability/groups.html#defining-groups-and-enabling-group-failover]



=== ディスク障害に対するフェイルオーバー

ディスク障害に対する自動フェイルオーバーの機能が提供されており、クラスター単位で有効化設定を行うことができます。


====[column]エディションによる差異

ディスク障害に対するフェイルオーバーは、エンタープライズエディションでのみ使用できます。

====[/column]

=== 自動フェイルオーバー設定

自動フェイルオーバー(Node Availability)設定は、Webコンソール@<fn>{general-settings-node-availability}、CLI@<fn>{auto-failover-settings-via-cli}、またはREST API@<fn>{node-availability-settings-via-rest}で行うことができます。

//footnote[general-settings-node-availability][https://docs.couchbase.com/server/current/manage/manage-settings/general-settings.html#node-availability]

//footnote[auto-failover-settings-via-cli][https://docs.couchbase.com/server/current/manage/manage-settings/general-settings.html#auto-failover-settings-via-cli]

//footnote[node-availability-settings-via-rest][https://docs.couchbase.com/server/current/manage/manage-settings/general-settings.html#node-availability-settings-via-rest]


== 手動フェイルオーバー

Couchbase Serverのフェイルオーバーは、以下の方法で手動で実行できます。

=== Webコンソール

 WebコンソールのServersメニューを選択し、ノード一覧を表示します。 フェイルオーバーしたいノードの「Fail Over」ボタンをクリックし、ノードをフェイルオーバーします。

=== コマンドラインツール

@<tt>{couchbase-cli}の@<tt>{failover}サブコマンドを利用して、ノードをフェイルオーバーすることができます。@<fn>{couchbase-cli-failover}

ノードをフェイルオーバーするには、フェイルオーバーするノードのIPアドレス（および標準ポート番号を利用していない場合はポート番号）を指定します。


//emlist{
couchbase-cli failover --cluster=<server(available)>:8091\
-u cluster-username -p cluster-password\
--server-failover=<server(target)>:8091
//}

//footnote[couchbase-cli-failover][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli-failover.html]

=== REST API

REST APIを利用して、フェイルオーバーを実行することができます。@<fn>{rest-node-failover}

以下に、@<tt>{curl}コマンドを用いた呼び出し方法を示します。

//emlist{
curl -v -X POST -u <username>:<password>
  http://<ip-address-or-hostname>:<port>/controller/failOver
  -d [otpNode=node_1@<hostname>]
  -d allowUnsafe=< true | false >
//}

//footnote[rest-node-failover][https://docs.couchbase.com/server/current/rest-api/rest-node-failover.html]

====[column]Couchbase Serverにおけるグレースフルフェイルオーバーとハードフェイルオーバー

Couchbase Serverでは、手動でフェイルオーバーを実行する場合(つまり、Webコンソール、コマンドライン、REST APIのいずれにも)、グレースフルフェイルオーバー(Graceful Failover@<fn>{perform-failover-graceful})とハードフェイルオーバー(Hard Failover@<fn>{perform-failover-hard})の両方の手段が用意されています。

グレースフルフェイルオーバーは、応答しているノードをクラスターから削除する際に用いられます。その場合、Couchbase Serverにはクラスターの削除という手段も存在します。それぞれの長所と短所を含め、グレースフルフェイルオーバーについて理解するために、ドキュメント@<fn>{couchbase-graceful-failover}を参照することができます。

ハードフェイルオーバーは、応答していないノードをクラスターから削除する際に用いられます。ハードフェイルオーバーの詳細について、ドキュメント@<fn>{couchbase-hard-failover}を参照することができます。

====[/column]



//footnote[perform-failover-graceful][https://docs.couchbase.com/server/current/manage/manage-nodes/failover-graceful.html]

//footnote[perform-failover-hard][https://docs.couchbase.com/server/current/manage/manage-nodes/failover-hard.html]


//footnote[couchbase-graceful-failover][https://docs.couchbase.com/server/current/learn/clusters-and-availability/graceful-failover.html]

//footnote[couchbase-hard-failover][https://docs.couchbase.com/server/current/learn/clusters-and-availability/hard-failover.html]