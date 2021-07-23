import 'package:mqtt_client/mqtt_client.dart';
import 'package:flutter_mqtt_app/covidappclient/MQTTCovidAppState.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class CovidAppManager {
  
  final MQTTCovidAppState covidAppCurrentState;
  MqttServerClient? covidAppClient;
  final String covidAppIdentifier;
  final String covidAppHost;
  final String covidAppTopic;

  //constructor
  CovidAppManager(
      {required String host,
      required String topic,
      required String identifier,
      required MQTTCovidAppState state})
      : covidAppIdentifier = identifier,
        covidAppHost = host,
        covidAppTopic = topic,
        covidAppCurrentState = state;

  // initialize the client
  void initializeMQTTClient() {
    covidAppClient = MqttServerClient(covidAppHost, covidAppIdentifier);
    covidAppClient!.port = 1883;
    covidAppClient!.keepAlivePeriod = 25;
    covidAppClient!.onDisconnected = onDisconnected;
    covidAppClient!.secure = false;
    covidAppClient!.logging(on: true);

    covidAppClient!.onConnected = onConnected;
    covidAppClient!.onSubscribed = onSubscribed;

    final MqttConnectMessage covidAppConnectionMessage = MqttConnectMessage()
        .withClientIdentifier(covidAppIdentifier)
        .withWillTopic(
            'willtopic') 
        .withWillMessage('My Will message')
        .startClean() 
        .withWillQos(MqttQos.atLeastOnce);
    print('mosquitto client connecting to University broker');
    covidAppClient!.connectionMessage = covidAppConnectionMessage;
  }

  // receive the message from the broker
    void onConnected() {
    covidAppCurrentState.setAppConnectionState(MQTTCovidAppConnectionState.connected);
    print('mosquitto client connected to the university broker');
    covidAppClient!.subscribe(covidAppTopic, MqttQos.atLeastOnce);
    covidAppClient!.updates!.listen((List<MqttReceivedMessage<MqttMessage?>>? mymessage) {

      final MqttPublishMessage receiveMessage = mymessage![0].payload as MqttPublishMessage;

      final String pt =
          MqttPublishPayload.bytesToStringAsString(receiveMessage.payload.message!);
      covidAppCurrentState.setReceivedText(pt);
    });
    print(
        'Covid app Client connection is sucessful');
  }

  //
 void publish(String message) {
    final MqttClientPayloadBuilder builder = MqttClientPayloadBuilder();
    builder.addString(message);
    covidAppClient!.publishMessage(covidAppTopic, MqttQos.exactlyOnce, builder.payload!);
  }
  
  void connect() async {
    assert(covidAppClient != null);
    try {
      print('mosquitto start client connecting to the university broker');
      covidAppCurrentState.setAppConnectionState(MQTTCovidAppConnectionState.connecting);
      await covidAppClient!.connect();
    } on Exception catch (exception) {
      print('client exception thrown as $exception');
      disconnect();
    }
  }

  void onSubscribed(String topic1) {
    print('subscription confirmed for topic $topic1');
  }

  void onDisconnected() {
    print('covid Client disconnection from the broker');
    if (covidAppClient!.connectionStatus!.returnCode ==
        MqttConnectReturnCode.noneSpecified) {
      print('onDisconnected callback is asked ');
    }
    covidAppCurrentState.setAppConnectionState(MQTTCovidAppConnectionState.disconnected);
  }

  void disconnect() {
    print('Disconnected from the broker');
    covidAppClient!.disconnect();
  }

}
