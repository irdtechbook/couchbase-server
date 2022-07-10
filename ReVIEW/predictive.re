
= 予測クエリ

エンタープライズエディションで提供される機能から、独自のユースケースに対応する機能を紹介します。

//blankline

Couchbase Liteは、予測クエリ(Predictive Query@<fn>{lbl-predquery})機能を提供します。
予測クエリについて、その登場の背景との関係を踏まえながら、紹介します。

//footnote[lbl-predquery][https://docs.couchbase.com/couchbase-lite/current/java/querybuilder.html#lbl-predquery]

== 背景

機械学習、深層学習の成果が実用化される時、多くの場合、そのサービスは、クラウドとのやりとりで実現されてきたといって良いでしょう。
そのようなサービスの例として、例えば、スマートスピーカーや、Web上やデスクトップアプリケーションで提供される自動翻訳機能があります。
このようなサービスは、データがクラウドに送られることで成立しています。
一方、iPhoneのFace IDでの顔認識モデルは、端末上で実現されています。
このような端末上でのAI機能は、エッジAIと言われています。

エッジAIの文脈において重要な要素として、セキュリティやプライバシーの観点があります。

機械学習にはモデルの学習と推論の二面がありますが、学習済みのモデルを使った「推論」機能を、エッジ環境で提供することにより、データをサーバーへ送信する必要がなくなります。
また、局所的な学習結果を連合(Federate)して一つのモデルを構築する、Federated Learning@<fn>{Federated_learning}のような、モデルの「学習」のためのテクノロジーも登場しています。


このようなニーズや背景を踏まえ、TensorFlow Lite@<fn>{tensorflow_lite}やPyTorch Mobile@<fn>{pytorch-mobile}のような、エッジAIを実現するためのオープンソースのソフトウェアライブラリが存在しています。
また、iOSのCoreML@<fn>{coreml}や、AndroidのNeural Networks API(NNAPI)@<fn>{android_nnapi}のような、プラットフォームサポートも登場しています。これらは、プラットフォーム固有の開発言語における機械学習の利用手段(API)を提供するだけでなく、テンソル演算専用のチップセットの活用のように、ハードウェアレベルでの最適化についても、実現できるようになっています。
その意味では、ハードウェアにおける深層学習への対応の波が、モバイルやIoTの世界にも及んでいる、という背景が前提にあります。
このような関係は、Google Coral@<fn>{coral-ai}が提供するデバイスに搭載されているEdge TPU@<fn>{edge-tpu}とTensor Flowとの関係にも現れています。

さらには、Apache TVM@<fn>{tvm-apache}のように、TensorFlow, PyTorch他各種ディープラーニングフレームワークと、CPU、GPU他各種ハードウェアリソースを横断的にサポートし、モデルと実行時間の最適化を実現するテクノロジーも登場しており、最適化のターゲットとしてモバイルデバイスもカバーされています。

Couchbase Liteの予測クエリは、エッジでの学習済みモデルを使った推論のためのものであり、このようなハードウェアおよびソフトウェアライブラリの隆盛と本質的な関係があります。

//footnote[Federated_learning][https://en.wikipedia.org/wiki/Federated_learning]
//footnote[android_nnapi][https://developer.android.com/ndk/guides/neuralnetworks]
//footnote[tensorflow_lite][https://tensorflow.google.cn/lite/guide]
//footnote[coreml][https://developer.apple.com/documentation/coreml]

//footnote[pytorch-mobile][https://pytorch.org/mobile/home/]

//footnote[tvm-apache][https://tvm.apache.org/]

//footnote[coral-ai][https://coral.ai/]

//footnote[edge-tpu][https://cloud.google.com/edge-tpu]

== 機能

=== 概要

予測クエリを使うことで、Couchbase Liteデータベースに格納されたデータと機械学習モデルとを容易に組み合わせることができるようになります。

予測クエリは、CoreMLやTensorFlow Lite、PyTorch Mobileのようなエッジデバイス上での機械学習モデルによる推論機能を提供するライブラリーとの組み合わせて利用します。
なお、予測クエリは、Android Java等、C言語以外の他のSDKでも利用できますが、Swift SDKでは、Core MLに準拠したAPIが提供されています。

この機能により、Couchbase Liteデータベースのデータ(イメージデータや、テキスト、数値)へ機械学習モデルによる推論を適用する際に、データ個々に対してコードを記述する必要がなくなります。
言い換えれば、データベースに対してモデルをアタッチすることで、データセットに対して推論結果を付与することが可能になります。

予測クエリを実行するには、次の手順を実装します。

//blankline

 1. モデルを登録する
 2. インデックスを作成する
 3. 予測クエリを実行する

//blankline

モデルを登録するには、@<tt>{PredictiveModel}インターフェイスを実装したクラスのインスタンスを作成してデータベースに登録します。


=== 位置づけ

先に予測クエリで実現できることとして、「データベースに対してモデルをアタッチすることで、データセットに対して推論結果を付与する」と書きました。
ここで、アタッチするモデルは、特定の仕様に縛られるものではなく、実際には機械学習モデルを内部で利用している通常の関数を登録します。
これにより、どのような実装とも組み合わせることのできる柔軟性を確保しています。
一方、ここでアタッチするものが予測モデル＝推論関数であることは技術的には本質的ではなく、何らかの任意の関数であっても成立するともいえます。

このことを踏まえると、単に技術的実装面からの発想ではなく、エッジAIという具体的なニーズ(今後のモバイル・エッジ端末の機能としてさらに重要な部分を占めることになるであろうもの)への対応として、このような機能が登場したということが読み取れるのではないかと思います。

=== 開発参考情報

予測クエリについて、下記のようなリソースが参照可能です。

//blankline


 * Couchbase Blog: Machine Learning Predictions in Mobile Apps with Couchbase Lite Predictive Query@<fn>{machine-learning-predictions-couchbase-lite-predictive-query}
 * Couchbase Labsサンプルアプリケーション: PredictiveQueriesWithCouchbaseLite@<fn>{couchbase-lite-predictive-query-examples}
 * APIリファレンス: CoreMLPredictiveModel@<fn>{CoreMLPredictiveModel}


//footnote[machine-learning-predictions-couchbase-lite-predictive-query][https://blog.couchbase.com/machine-learning-predictions-couchbase-lite-predictive-query/]
//footnote[couchbase-lite-predictive-query-examples][https://github.com/couchbaselabs/couchbase-lite-predictive-query-examples]
//footnote[CoreMLPredictiveModel][https://docs.couchbase.com/mobile/3.0.0/couchbase-lite-swift/Classes/CoreMLPredictiveModel.html]

