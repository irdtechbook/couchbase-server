
= Couchbase Lite C言語サポート

== 概要

Couchbase Liteは、C言語での利用がサポートされています。@<fn>{couchbase-c-support}

//footnote[couchbase-c-support][https://docs.couchbase.com/couchbase-lite/current/c/]

=== サポートプラットフォーム


Couchbase Liteは、C言語での利用環境として、Unix系OSとしては、macOS、Ubuntu、Debianの他、Raspberry Pi OSをサポートしています。

また、Windows、Android、iOSもサポートしており、これらの環境用のアプリケーションをC/C++でプログラミングする際にCouchbase Liteを利用することが可能です。

以下に、サポート対象のOSのバージョンとプロセッサーのアーキテクチャー種類を整理します。

//table[tbl5][Couchbase Lite C APIサポートプラットフォーム]{
OS\アーキテクチャー	x64	ARM32	ARM64
-----------------
Debian 9	N/A	サポート	サポート
Debian 9 Desktop	サポート	N/A	N/A	
Debian 10	N/A	サポート	サポート
Debian 10 Desktop	サポート	N/A	N/A
Ubuntu 20.04 Desktop	サポート	N/A	N/A
Ubuntu 20.04 Core	N/A	N/A	サポート
Raspberry Pi OS9 (stretch)	N/A	サポート	N/A
Raspberry Pi OS10 (Buster)	N/A	サポート	サポート
iOS10+	サポート	N/A	サポート
Android (API22+)	サポート	サポート	サポート
macOS (Catalina+)	サポート	N/A	サポート
Windows 10	サポート	N/A	N/A
//}

=== アーキテクチャー


Couchbase Liteは、2018年にリリースされたバージョン2.0にて、C/C++を用いて再実装されました。
現在のCouchbase Liteはプラットフォームに依存しない共通のコアコンポーネントに基づいています。
Java、Swift、Objective-C、C#等、各プラットフォーム用のCouchbase Lite実装は、全て内部でC APIを利用しています。

Couchbase Lite C言語実装は、 Couchbase Liteコアライブラリと静的にリンクされたバイナリとして提供されます。

//image[c-support-arch][C API階層アーキテクチャー]{
//}

(図は、Couchbase Blog: Couchbase Lite — In C!@<fn>{couchbase-lite-in-c}より引用）

//blankline

//footnote[couchbase-lite-in-c][https://blog.couchbase.com/couchbase-lite-in-c/]



== 利点

=== C言語の利点

C言語特有の以下の利点があります。

//blankline

 * @<em>{リソースフットプリントと実行速度}: C/C++を用いて開発することにより、IoT/エッジコンピューティングに求められる、低いリソースフットプリントと実行速度の最大化が図れます。
 * @<em>{非サポート言語用のパッケージ開発}: C APIを他のプログラミング言語と組み合わせて利用することによって、開発プラットフォームに幅広い選択肢をもたらすことが考えられます。例えば、Python、JavaScript、Rust、Goなどのプログラミング言語による開発の際に、FFI(Foreign function interface@<fn>{Foreign_function_interface})のような、それらの言語が提供するネイティブ言語サポートを活用し、Couchbase Liteを用いたアプリケーションを構築することができます。

//footnote[Foreign_function_interface][https://en.wikipedia.org/wiki/Foreign_function_interface]

=== フィールドレベル暗号化

Couchbase Lite C APIは、フィールドレベル暗号化(Field Level Encryption@<fn>{field-level-encryption})機能を提供します。@<fn>{memo-field-level-encryption}
フィールドレベル暗号化は、Couchbase Lite 3.0の時点では、他のプログラミング言語ではサポートされていません。

クライアント側の暗号化機能を使用すると、データをCouchbase Serverと同期するクライアントアプリケーションとしてCouchbase Liteを利用する際に、ネットワーク経由で複製するドキュメント内のフィールドを暗号化することができます。正しい暗号化キーにアクセスできるクライアントのみが、データを復号化して読み取ることができます。

Couchbase Liteのフィールドレベル暗号化は、Couchbase Server SDKのフィールドレベル暗号化と互換性があります。

//footnote[field-level-encryption][https://docs.couchbase.com/couchbase-lite/current/c/field-level-encryption.html]
//footnote[memo-field-level-encryption][フィールドレベル暗号化はエンタープライズエディションで提供される機能です。]