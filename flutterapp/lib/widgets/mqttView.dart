import 'dart:convert';
import 'dart:io' show Platform;
import 'package:sparkline_console/sparkline_console.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:flutter_mqtt_app/mqtt/state/MQTTAppState.dart';
import 'package:flutter_mqtt_app/mqtt/MQTTManager.dart';
//import "package:oscilloscope/oscilloscope.dart";
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_charts/sparkcharts.dart';


class MQTTView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return _MQTTViewState();
  }
}

class _MQTTViewState extends State<MQTTView> {
  final TextEditingController _hostTextController = TextEditingController();
  final TextEditingController _messageTextController = TextEditingController();
  final TextEditingController _topicTextController = TextEditingController();
  late MQTTAppState currentAppState;
  late MQTTManager manager;
  List<int> nums = List.filled(10,0);
  List<_SData> data = [
  ];

  @override
  void initState() {
    super.initState();

    /*
    _hostTextController.addListener(_printLatestValue);
    _messageTextController.addListener(_printLatestValue);
    _topicTextController.addListener(_printLatestValue);

     */
  }

  @override
  void dispose() {
    _hostTextController.dispose();
    _messageTextController.dispose();
    _topicTextController.dispose();
    super.dispose();
  }

  /*
  _printLatestValue() {
    print("Second text field: ${_hostTextController.text}");
    print("Second text field: ${_messageTextController.text}");
    print("Second text field: ${_topicTextController.text}");
  }

   */

  @override
  Widget build(BuildContext context) {
    final MQTTAppState appState = Provider.of<MQTTAppState>(context);
    // Keep a reference to the app state.
    currentAppState = appState;
    final Scaffold scaffold = Scaffold(
        appBar:  AppBar(
            title: Text('COVID ROOM OCCUPANCY'),
            centerTitle: true,
            backgroundColor: Colors.blueGrey[600]),
        body: _buildColumn());
    return scaffold;
  }

  Widget _buildAppBar(BuildContext context) {
    return AppBar(
      title: const Text('COVID ROOM OCCUPANCY-MQTT'),
      backgroundColor: Colors.purple,
      centerTitle: true,
    );
  }

  Widget _buildColumn() {
    return Column(
      children: <Widget>[
        _buildScrollableTextWith(currentAppState.getReceivedText),
        _buildEditableColumn(),
        _buildConnectionStateText(
            _prepareStateMessageFrom(currentAppState.getAppConnectionState))
      ],
    );
  }

  Widget _buildEditableColumn() {
    return Padding(
      padding: const EdgeInsets.all(20.0),
      child: Column(
        children: <Widget>[
  //        _buildTextFieldWith(_hostTextController, 'Enter broker address',
  //            currentAppState.getAppConnectionState),
 //         const SizedBox(height: 10),
  //        _buildTextFieldWith(
  //            _topicTextController,
  //            'Enter a topic to subscribe or listen',
  //            currentAppState.getAppConnectionState),
  //        const SizedBox(height: 10),
  //        _buildPublishMessageRow(),
          const SizedBox(height: 10),
          _buildConnecteButtonFrom(currentAppState.getAppConnectionState)
        ],
      ),
    );
  }

  Widget _buildPublishMessageRow() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
      children: <Widget>[
        Expanded(
          child: _buildTextFieldWith(_messageTextController, 'Enter a message',
              currentAppState.getAppConnectionState),
        ),
        _buildSendButtonFrom(currentAppState.getAppConnectionState)
      ],
    );
  }

  Widget _buildConnectionStateText(String status) {
    return Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      mainAxisSize: MainAxisSize.max,
      mainAxisAlignment: MainAxisAlignment.end,
      children: <Widget>[
        Expanded(
          child: Container(
              padding: EdgeInsets.all(30),
              color: Colors.lightGreenAccent,
              child: Text(status, textAlign: TextAlign.center)),
        ),
      ],
    );
  }

  Widget _buildTextFieldWith(TextEditingController controller, String hintText,
      MQTTAppConnectionState state) {
    bool shouldEnable = false;
    if (controller == _messageTextController &&
        state == MQTTAppConnectionState.connected) {
      shouldEnable = true;
    } else if ((controller == _hostTextController &&
            state == MQTTAppConnectionState.disconnected) ||
        (controller == _topicTextController &&
            state == MQTTAppConnectionState.disconnected)) {
      shouldEnable = true;
    }
    return TextField(
        enabled: shouldEnable,
        controller: controller,
        decoration: InputDecoration(
          contentPadding:
              const EdgeInsets.only(left: 0, bottom: 0, top: 0, right: 0),
          labelText: hintText,
        ));
  }

  Widget _buildScrollableTextWith(String text) {
   // var sparkLine = new SparkLine();
    DateTime now = DateTime.now();
    String mytime = now.hour.toString() + ":" + now.minute.toString() + ":" + now.second.toString();
    String numberofPeople = text.replaceAll(new RegExp(r'-?[^0-9]'),''); // '23'
    if(numberofPeople.isNotEmpty) {
      int mynumber = int.parse(numberofPeople);
      data.add(_SData(mytime, mynumber));
    }else{
      data.add(_SData(mytime, 0));
    }
    try {
      //nums.add(int.parse(numberofPeople));

      //String year_ = now.minute.toString();
    }on Exception catch (_) {
      print("exception caught");
    }

  //  String numberofPeople = json[0]["values"];
    return Column(children: [
      //Initialize the chart widget
      SfCartesianChart(
          primaryXAxis: CategoryAxis(),
          // Chart title
          title: ChartTitle(text: 'Number of people'),
          // Enable legend
          //legend: Legend(isVisible: true),
          // Enable tooltip
          //tooltipBehavior: TooltipBehavior(enable: true),
          series: <ChartSeries<_SData, String>>[
            FastLineSeries<_SData, String>(
                dataSource: data,
                xValueMapper: (_SData sales, _) => sales.year,
                yValueMapper: (_SData sales, _) => sales.number,
                //name: 'Number of people',
                // Enable data label
               // dataLabelSettings: DataLabelSettings(isVisible: true))
          )],
    )]);
  }

  Widget _buildConnecteButtonFrom(MQTTAppConnectionState state) {
    return Row(
      children: <Widget>[
        Expanded(
          // ignore: deprecated_member_use
          child: RaisedButton(
            padding:EdgeInsets.all(20),
            color: Colors.blueGrey,
            child: Row(
                children:<Widget>[
                Icon(Icons.network_wifi),
                const Text('MQTTConnect')
    ]),
            onPressed: state == MQTTAppConnectionState.disconnected
                ? _configureAndConnect
                : null, //
          ),
        ),
        const SizedBox(width: 10),
        Expanded(
          // ignore: deprecated_member_use
          child: RaisedButton(
            padding:EdgeInsets.all(20),
            color: Colors.redAccent,
            child: Row(
                children:<Widget>[
                  Icon(Icons.network_check),
                  const Text('Disconnect')
                ]),
            onPressed: state == MQTTAppConnectionState.connected
                ? _disconnect
                : null, //
          ),
        ),
      ],
    );
  }

  Widget _buildSendButtonFrom(MQTTAppConnectionState state) {
    // ignore: deprecated_member_use
    return RaisedButton(
      color: Colors.green,
      child: const Text('Send'),
      onPressed: state == MQTTAppConnectionState.connected
          ? () {
              _publishMessage(_messageTextController.text);
            }
          : null, //
    );
  }

  // Utility functions
  String _prepareStateMessageFrom(MQTTAppConnectionState state) {
    switch (state) {
      case MQTTAppConnectionState.connected:
        return 'Connected';
      case MQTTAppConnectionState.connecting:
        return 'Connecting';
      case MQTTAppConnectionState.disconnected:
        return 'Disconnected';
    }
  }

  void _configureAndConnect() {
    // ignore: flutter_style_todos
    // TODO: Use UUID
    String osPrefix = 'Flutter_iOS';
    if (Platform.isAndroid) {
      osPrefix = 'Flutter_Android';
    }
    //TODO:
    manager = MQTTManager(
       // host: _hostTextController.text, // earth.informatik.uni-freiburg.de
        host: 'earth.informatik.uni-freiburg.de',
       // topic: _topicTextController.text, //ubiblab/sensor/occupancy
        topic:'ubiblab/sensor/occupancy',
        identifier: osPrefix,
        state: currentAppState);
    manager.initializeMQTTClient();
    manager.connect();
  }

  void _disconnect() {
    manager.disconnect();
  }

  void _publishMessage(String text) {
    String osPrefix = 'Flutter_iOS';
    if (Platform.isAndroid) {
      osPrefix = 'Flutter_Android';
    }
    final String message = osPrefix + ' says: ' + text;
    manager.publish(message);
    _messageTextController.clear();
  }
}

class _SData {
  _SData(this.year, this.number);

  final String year;
  final int number;
}