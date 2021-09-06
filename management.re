
= 運用


== 監視

Couchbase Serverにおける監視について見ていきます。

Couchbase Serverの監視機能の対象は、CPU・メモリ利用量などハードウェア情報に関するノード（サーバー）レベルの監視と、データ量やアクセス状況などデータベースに関するクラスターやバケットレベルの監視の両方を含んでいます。


=== Webコンソール

Couchbase ServerのWebコンソールは、様々な情報を期間やレベルを変えながら表示することのできるダッシュボードを備えています。
//image[web_console_monitoring][]{
//}


=== Eメールアラート

Couchbase Serverは、（自動フェールオーバー等）特定の状況が発生した際に、Eメールを送付する機能を持っており、Webコンソール画面またはCLI、REST APIから設定することが可能です。

=== Rest APIとCLI

外部システムとの汎用的な連携をサポートするために、Couchbase Serverは、Rest APIとCLIによる監視情報へのアクセスを提供しています。
Webコンソールで公開されている監視情報は、全てRest APIを使って取得することが可能です。
監視システムとの連携の手法として、（エージェント型監視ツールを典型として）CLI(コマンドラインインターフェイス)　が必要とされるケースもあります。Couchbase Serverは用途に応じた各種CLIツールを備えていますが、監視情報取得の用途として@<tt>{cbstats}があります。

=== Prometheus連携

Prometheusは、監視対象にアクセスしてデータを収集するPull型アーキテクチャに基づいています。そして、監視対象サーバーは、Exporterと呼ばれるアクセスポイントを公開します。下記で、公開されているExporterのリストを見ることができます。

Prometheus公式ドキュメント Exporters and Integrations@<fn>{exporters}

//footnote[exporters][https://prometheus.io/docs/instrumenting/exporters/]

Couchbase Githubリポジトリ couchbase-exporter@<fn>{couchbase-exporter}

//footnote[couchbase-exporter][https://github.com/couchbase/couchbase-exporter]


Couchbase Exporterは、監視するCouchbase Serverクラスターに対して、プロセスを実行します。

//cmd{
./couchbase-exporter --couchbase.username Administrator --couchbase.password password --web.listen-address=":9420" --couchbase.url="http://example.host.a:8091"
//}

また、Prometeusで収集したデータを可視化するための、Grafanaのダッシュボード定義のサンプル@<fn>{GrafanaDashboard}が公開されています。


//footnote[GrafanaDashboard][https://github.com/couchbaselabs/blog-source-code/blob/master/Meliani/GrafanaDashboard.json]


//image[grafana][]{
//}

=== 参考情報

Couchbase公式ドキュメント Monitoring with UI@<fn>{ui-monitoring-statistics}

//footnote[ui-monitoring-statistics][https://docs.couchbase.com/server/current/manage/monitor/ui-monitoring-statistics.html]

Couchbaseブログ Couchbase Monitoring@<fn>{monitoring-couchbase-cluster}

//footnote[monitoring-couchbase-cluster][https://blog.couchbase.com/monitoring-couchbase-cluster/]

Couchbaseブログ Couchbase Monitoring Integration with Prometheus and Grafana@<fn>{couchbase-monitoring-integration-with-prometheus-and-grafana}

//footnote[couchbase-monitoring-integration-with-prometheus-and-grafana][https://blog.couchbase.com/couchbase-monitoring-integration-with-prometheus-and-grafana/ CouchbaseとPrometheus,Grafanaとの連携に関する詳細な手順説明]



== バックアップ

どんなデータベースにとっても、バックアップは必須の機能です。

Couchbase Serverのバックアップのために用意されている、@<tt>{cbbackupmgr}コマンドの使用例を紹介します。



このチュートリアルの実行例では、@<tt>{travel-sample}バケットと@<tt>{beer-sample}バケットを利用します。


=== バックアップリポジトリの作成


@<tt>{cbbackupmgr}の使用を開始するにあたって、まずバックアップを保存するディレクトリを決定する必要があります。このディレクトリは、@<strong>{バックアップアーカイブ}と呼ばれます。バックアップアーカイブには、複数の@<strong>{バックアップリポジトリ}が含まれることになります。
バックアップリポジトリを構成する最も簡単な方法は、単一のクラスターに直接対応させることです。
バックアップリポジトリは、@<tt>{config}サブコマンドを使用して作成されます。このチュートリアルでは、@<tt>{/data/backup}にあるバックアップアーカイブを使用します。指定されたディレクトリが空の場合、バックアップアーカイブは自動的に作成されます。以下は、「cluster」という名前でバックアップリポジトリを作成する例です。


//emlist[][shell]{
$ cbbackupmgr config -a /data/backup -r cluster

Backup repository `cluster` created successfully in archive `/data/backup`
//}


バックアップリポジトリ 「cluster」がアーカイブ@<tt>{/data/backup}に正常に作成されました



バックアップリポジトリを作成する際に、バックアップ方法を変更することができます。
例えば、@<tt>{travel-sample}バケット内のインデックス定義のみをバックアップするとします。これを行うためには以下のオプションを使い、ここでは「single」という名前でバックアップリポジトリを作成します。


//emlist[][shell]{
$ cbbackupmgr config -a /data/backup -r single \
  --include-data travel-sample --disable-data

Backup repository `single` created successfully in archive `/data/backup`
//}


@<tt>{config}サブコマンドは、データのバックアップ方法をカスタマイズするための多くのオプションを提供します。使用可能なオプションとその使用方法の詳細については、ドキュメントをご参照ください。


=== バックアップ構成の確認


バックアップリポジトリを作成したので、バックアップアーカイブがどのように見えるかを確認します。そのために、@<tt>{info}サブコマンドを使用することができます。


//emlist[][shell]{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size  | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 0B    | 2        |
*  Name     | Size  | # Backups  |
*  cluster  | 0B    | 0          |
//}


@<tt>{info}サブコマンドは、バックアップアーカイブ内のすべてのバックアップリポジトリとバックアップのディレクトリを表示します。ここでは、バックアップはまだ作成されていないので、このコマンドの出力でアーカイブ情報のみが確認できます。各リポジトリに含まれるディスク容量に関する情報もあります。


=== バックアップの作成


バックアップリポジトリが構成されたので、バックアップの作成を開始します。バックアップリポジトリには、バックアップの作成方法に関するすべての構成情報が含まれているため、バックアップ作成時には、バックアップリポジトリ名とバックアップするターゲットクラスターの情報を指定するだけです。以下は、「cluster」バックアップリポジトリでバックアップを作成する方法の例です。@<tt>{localhost}でクラスターが実行されていると想定します。


//emlist[][shell]{
$ cbbackupmgr backup -c 127.0.0.1 -u Administrator -p password -a /data/backup -r cluster
Backing up to 2020-03-25T08_08_11.770436Z
Copied all data in 33.02s (Avg. 759.44KB/Sec)  38894 items / 24.47MB
beer-sample             [===================================] 100.00%
travel-sample           [===================================] 100.00%

Backup successfully completed
Backed up bucket "beer-sample" succeeded
Mutations backed up: 7303, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
Backed up bucket "travel-sample" succeeded
Mutations backed up: 31591, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


コマンドを実行すると、プログレスバーが出力されます。これは、バックアップの完了にかかる時間とデータ移動の速度を理解するのに役立ちます。バックアップの実行中、プログレスバーは完了までの推定時間を示しますが、バックアップが完了すると、これは平均バックアップレートに変更されます。すでにバックアップされているデータとアイテムの合計、および現在のデータ移動速度に関する情報も提供されます。バックアップが正常に完了すると、「Backup successfully completed」というメッセージと、バケットごとにバックアップされた変更と削除の内訳が表示されます。



「single」バックアップリポジトリでも、バックアップを実行して、2つのバックアップ実行の違いを確認してみましょう。


//emlist[][shell]{
$ cbbackupmgr backup -a /data/backup -r single \
  -c couchbase://127.0.0.1 -u Administrator -p password

Backing up to 2020-03-25T08_08_58.903046Z
Copied all data in 1s (Avg. 480B/Sec)                 0 items / 480B
travel-sample           [==================================] 100.00%
Backup successfully completed
Backed up bucket "travel-sample" succeeded
Mutations backed up: 0, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


「single」バックアップリポジトリは、@<tt>{travel-sample}バケットのインデックス定義をバックアップするようにのみ構成されているため、@<tt>{beer-sample}バケットの進行状況バーは表示されません。また、バックアップするデータがはるかに少ないため、バックアップがより迅速に実行されたことがわかります。



これで、バックアップアーカイブにバックアップが作成されたので、infoサブコマンドを使用してバックアップアーカイブの状態が変更されたことを確認してみましょう。


//emlist[][shell]{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 54.35MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 54.33MB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_11.770436Z  | 54.33MB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31591  | 31591      | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}


いくつかのバックアップが作成されたので、@<tt>{info}サブコマンドの出力内容が変わっています。「cluster」バックアップリポジトリには、バックアップが作成された時間に対応する名前のバックアップが1つ含まれていることがわかります。そのバックアップには2つのバケットも含まれており、バケット内のビュー、インデックス、ミューテーション、トゥームストーンなどの数を確認できます。「single」バックアップリポジトリにも1つのバックアップが含まれていますが、このバックアップには@<tt>{travel-sample}バケットのみが含まれ、データ項目が0個であり、10個のインデックスがあることがわかります。なお、@<tt>{info}コマンドはJSON出力もサポートしています。


=== 増分バックアップ


@<tt>{cbbackupmgr}の重要な機能の1つは、増分バックアップです。既にバックアップされているデータは、再度バックアップする必要はありません。
確認のため、一部のデータを変更した後、「cluster」バックアップリポジトリで@<tt>{backup}サブコマンドを再度実行してみます。


//emlist[][shell]{
$ cbbackupmgr backup -a /data/backup -r cluster -c couchbase://127.0.0.1 -u Administrator -p password

Backing up to 2020-03-25T08_41_21.461311Z
Copied all data in 3s (Avg. 18.98KB/Sec)           4 items / 56.95KB
travel-sample           [==================================] 100.00%
beer-sample             [==================================] 100.00%

Backup successfully completed
Backed up bucket "beer-sample" succeeded
Mutations backed up: 0, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
Backed up bucket "travel-sample" succeeded
Mutations backed up: 4, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


この実行例では、バックアップ実行前に2つのアイテムを更新して2つのアイテムを作成しています。
バックアップされたのはそれらのデータのみであることに注意してください。



さらに、@<tt>{info}サブコマンドを使用してバックアップアーカイブを一覧表示すると、バックアップアーカイブは次のようになります。Itemsの数は4であり、そのすべてが@<tt>{Mutations}であることがわかります。これらは、2つの変更と2つの追加に対応しています。


//emlist[][shell]{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 86.37MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 86.35MB  | 2          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_11.770436Z  | 54.33MB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31591  | 31591      | 0           | 0      | 0    | 10       | 0     |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_41_21.461311Z  | 32.02MB  | INCR  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 16.00MB  | 0      | 0          | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.02MB  | 4      | 4          | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}


@<tt>{backup}サブコマンドは、データのバックアップ方法をカスタマイズするための多くのオプションを提供します。使用可能なオプションとその使用方法の詳細については、ドキュメントを参照してください。


=== バックアップの復元


バックアップデータができたので、そのバックアップデータをクラスターに復元してみましょう。データを復元するには、復元するバックアップの名前を知っている必要があります。名前を見つけるには、@<tt>{info}サブコマンドを使用して、バックアップアーカイブの内容を確認します。バックアップ名は常にタイムスタンプになります。たとえば、「cluster」バックアップリポジトリから「2016-03-22T10@<b>{26}08.933579821-07_00」を復元するとします。これを行うには、以下のコマンドを実行します。


//emlist[][shell]{
$ cbbackupmgr restore -a /data/backup -r cluster \
 -c http://127.0.0.1:8091 -u Administrator -p password \
 --start 2016-03-22T14_00_16.892277632-07_00 \
 --end 2016-03-22T14_00_16.892277632-07_00 --force-updates

(1/1) Restoring backup 2016-03-22T14_00_16.892277632-07_00
Copied all data in 2s (Avg. 19.96MB/Sec)       38894 items / 39.91MB
travel-sample           [==================================] 100.00%
beer-sample             [==================================] 100.00%

Restore completed successfully
//}


上記のコマンドでは、@<tt>{--start}フラグと@<tt>{--end}フラグを使用して、復元するバックアップの範囲を指定しています。バックアップを1つだけ復元するため、@<tt>{--start}と@<tt>{--end}の両方に同じ値を指定します。また、競合解決をスキップするために、@<tt>{--force-updates}フラグを追加しました。これにより、@<tt>{cbbackupmgr}は、クラスター上のキーと値のペアが新しく更新されている場合でも、キーと値のペアを強制的に上書きして復元するように指示されます。クラスターで更新した2つの値を見ると、最初のバックアップを作成したときの値に戻っていることがわかります。



ここでは詳細について触れませんが、@<tt>{restore}サブコマンドでは、バックアップされた一部のデータを復元から除外するなど、さまざまなオプションを利用することができます。


=== バックアップのマージ


増分バックアップを行うと、バックアップを作成するたびにディスク容量が増加します。ディスクスペースは無限ではないので、スペースを再利用できるようにすることが重要。これを行うには、@<tt>{merge} サブコマンドを使用して2つ以上のバックアップをマージします。「cluster」バックアップリポジトリには2つのバックアップがあるため、以下のコマンドを使用してこれらのバックアップをマージします。


//emlist[][shell]{
$ cbbackupmgr merge -a /data/backup -r cluster \
  --start oldest --end latest

Merge completed successfully
//}


バックアップをマージした後、@<tt>{list}サブコマンドを使用して、実行したばかりの@<tt>{merge}サブコマンドがバックアップアーカイブに与えた影響を確認できます。


//emlist[][shell]{
$ cbackupmgr info -a /data/backup --all                                                                                                                                                                                                                                                                                                                                                                                     20-03-25 - 8:52:36
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 54.35MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 54.33MB  | 1          |

+    Backup                       | Size     | Type          | Source  | Cluster UUID                      | Range                                      | Events  | Aliases  | Complete  |
+    2020-03-25T08_41_21.461311Z  | 54.33MB  | MERGE - FULL  | Merge   | ffa8024f899ca6acc7c59cf0f541dbdd  | 2020-03-25T08_08_11-2020-03-25T08_41_21:2  | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31593  | 31593      | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}


「cluster」バックアップリポジトリに単一のバックアップがあることがわかります。このバックアップの名前は、マージの最新のバックアップの名前を反映しています。また、@<tt>{travel-sample}バケットには31593個のデータ項目があります。2番目のバックアップに2つの新しいアイテムがあったため、これは元のバックアップより2つ多いアイテムです。更新された2つのアイテムは、マージ中に重複排除されたため、@<tt>{info}サブコマンドによって表示されるカウントに追加されません。


=== バックアップリポジトリの削除


バックアップリポジトリが不要になった場合は、@<tt>{remove}サブコマンドを使用してバックアップリポジトリを削除できます。以下は、「クラスター」バックアップリポジトリを削除する方法を示す例です。


//emlist[][shell]{
$ cbbackupmgr remove -a /data/backup -r cluster

Backup repository `cluster` deleted successfully from archive `/data/backup`
//}


バックアップリポジトリ 「cluster」がアーカイブ@<tt>{/data/backup}から正常に削除されました。
ここで@<tt>{list}サブコマンドを実行すると、「cluster」バックアップリポジトリが存在しなくなっていることがわかります。


=== 参考情報

Couchbase公式ドキュメント cbbackupmgr tutorial@<fn>{cbbackupmgr-tutorial}



//footnote[cbbackupmgr-tutorial][https://docs.couchbase.com/server/current/backup-restore/cbbackupmgr-tutorial.html]

