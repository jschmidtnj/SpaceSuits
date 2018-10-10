/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { AngularTestPage } from './app.po';
import { ExpectedConditions, browser, element, by } from 'protractor';
import {} from 'jasmine';


describe('Starting tests for admin-frontend', function() {
  let page: AngularTestPage;

  beforeEach(() => {
    page = new AngularTestPage();
  });

  it('website title should be admin-frontend', () => {
    page.navigateTo('/');
    return browser.getTitle().then((result)=>{
      expect(result).toBe('admin-frontend');
    })
  });

  it('network-name should be api2@0.0.1',() => {
    element(by.css('.network-name')).getWebElement()
    .then((webElement) => {
      return webElement.getText();
    })
    .then((txt) => {
      expect(txt).toBe('api2@0.0.1.bna');
    });
  });

  it('navbar-brand should be admin-frontend',() => {
    element(by.css('.navbar-brand')).getWebElement()
    .then((webElement) => {
      return webElement.getText();
    })
    .then((txt) => {
      expect(txt).toBe('admin-frontend');
    });
  });

  
    it('EHR component should be loadable',() => {
      page.navigateTo('/EHR');
      browser.findElement(by.id('assetName'))
      .then((assetName) => {
        return assetName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('EHR');
      });
    });

    it('EHR table should have 7 columns',() => {
      page.navigateTo('/EHR');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(7); // Addition of 1 for 'Action' column
      });
    });
  
    it('PatientInfo component should be loadable',() => {
      page.navigateTo('/PatientInfo');
      browser.findElement(by.id('assetName'))
      .then((assetName) => {
        return assetName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('PatientInfo');
      });
    });

    it('PatientInfo table should have 4 columns',() => {
      page.navigateTo('/PatientInfo');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(4); // Addition of 1 for 'Action' column
      });
    });
  
    it('DoctorInfo component should be loadable',() => {
      page.navigateTo('/DoctorInfo');
      browser.findElement(by.id('assetName'))
      .then((assetName) => {
        return assetName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('DoctorInfo');
      });
    });

    it('DoctorInfo table should have 5 columns',() => {
      page.navigateTo('/DoctorInfo');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(5); // Addition of 1 for 'Action' column
      });
    });
  

  
    it('Patient component should be loadable',() => {
      page.navigateTo('/Patient');
      browser.findElement(by.id('participantName'))
      .then((participantName) => {
        return participantName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('Patient');
      });
    });

    it('Patient table should have 6 columns',() => {
      page.navigateTo('/Patient');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(6); // Addition of 1 for 'Action' column
      });
    });
  
    it('Doctor component should be loadable',() => {
      page.navigateTo('/Doctor');
      browser.findElement(by.id('participantName'))
      .then((participantName) => {
        return participantName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('Doctor');
      });
    });

    it('Doctor table should have 4 columns',() => {
      page.navigateTo('/Doctor');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(4); // Addition of 1 for 'Action' column
      });
    });
  
    it('Insurance component should be loadable',() => {
      page.navigateTo('/Insurance');
      browser.findElement(by.id('participantName'))
      .then((participantName) => {
        return participantName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('Insurance');
      });
    });

    it('Insurance table should have 5 columns',() => {
      page.navigateTo('/Insurance');
      element.all(by.css('.thead-cols th')).then(function(arr) {
        expect(arr.length).toEqual(5); // Addition of 1 for 'Action' column
      });
    });
  

  
    it('ChangeInsurance component should be loadable',() => {
      page.navigateTo('/ChangeInsurance');
      browser.findElement(by.id('transactionName'))
      .then((transactionName) => {
        return transactionName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('ChangeInsurance');
      });
    });
  
    it('RemoveDoctorViewAccess component should be loadable',() => {
      page.navigateTo('/RemoveDoctorViewAccess');
      browser.findElement(by.id('transactionName'))
      .then((transactionName) => {
        return transactionName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('RemoveDoctorViewAccess');
      });
    });
  
    it('AddDoctorViewAccess component should be loadable',() => {
      page.navigateTo('/AddDoctorViewAccess');
      browser.findElement(by.id('transactionName'))
      .then((transactionName) => {
        return transactionName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('AddDoctorViewAccess');
      });
    });
  
    it('DoctorUpdatePatientData component should be loadable',() => {
      page.navigateTo('/DoctorUpdatePatientData');
      browser.findElement(by.id('transactionName'))
      .then((transactionName) => {
        return transactionName.getText();
      })
      .then((txt) => {
        expect(txt).toBe('DoctorUpdatePatientData');
      });
    });
  

});