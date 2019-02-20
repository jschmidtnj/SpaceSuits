import React, { Component } from 'react';
import axios from 'axios';
import { BootstrapTable, TableHeaderColumn } from 'react-bootstrap-table';

class Data extends Component {
  constructor(data) {
    super();
    // console.log(data.interval);
    this.state = {
      suitdata: {},
      taskdata: {},
      dataStr: '',
      interval: data.interval,
      getSuitDataUrl: data.getSuitDataUrl,
      getTaskDataUrl: data.getTaskDataUrl,
      products: [
        {
          id: 1,
          name: 'Item name 1',
          price: 100,
        },
        {
          id: 2,
          name: 'Item name 2',
          price: 100,
        },
      ],
    };
  }

  getData = () => {
    // url for testing:
    // response.data.message
    const testurl = 'https://dog.ceo/api/breeds/list/all';
    axios.get(testurl).then(response => {
      let message = response.data;
      let messageStr = JSON.stringify(message);
      console.log(messageStr);
      this.setState({
        data: message,
        dataStr: messageStr,
      });
    });
  };

  createTaskTable = tasksjson => {
    //console.log(tasksjson);
    if (Object.keys(tasksjson).length === 0) {
      return;
    }
    for (var majorkey in tasksjson) {
      for (var subkey in tasksjson[majorkey]) {
        var singletaskdata = tasksjson[majorkey][subkey];
        var tasktimeutc = parseInt(singletaskdata.time);
        console.log("time: " + tasktimeutc.toString());
        var tasktimeutcdate = new Date(tasktimeutc);
        var singletasktime = tasktimeutcdate.toString();
        console.log("date: " + singletasktime);
        var singletaskdatavalue = singletaskdata.data;
        console.log(majorkey + " -> " + subkey + " -> " + singletaskdatavalue);
        var tasknum = majorkey + '.' + subkey;
        console.log(tasknum);
      }
    }
  };

  componentDidMount() {
    this.getData();
    this.requestInterval = setInterval(this.getData, this.state.interval);
  }

  componentWillUnmount() {
    clearInterval(this.requestInterval);
  }

  render() {
    if (this.state.dataStr.length === 0) return <div />;
    return (
      <div>
        <p>{this.state.dataStr}</p>
        <BootstrapTable data={this.state.products} striped={true} hover={true}>
          <TableHeaderColumn
            dataField="id"
            isKey={true}
            dataAlign="center"
            dataSort={true}
          >
            Product ID
          </TableHeaderColumn>
          <TableHeaderColumn dataField="name" dataSort={true}>
            Product Name
          </TableHeaderColumn>
          <TableHeaderColumn dataField="price">Product Price</TableHeaderColumn>
        </BootstrapTable>
      </div>
    );
  }
}

export default Data;
