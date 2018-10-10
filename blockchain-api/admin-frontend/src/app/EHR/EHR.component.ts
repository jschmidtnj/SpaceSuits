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

import { Component, OnInit, Input } from '@angular/core';
import { FormGroup, FormControl, Validators, FormBuilder } from '@angular/forms';
import { EHRService } from './EHR.service';
import 'rxjs/add/operator/toPromise';

@Component({
  selector: 'app-ehr',
  templateUrl: './EHR.component.html',
  styleUrls: ['./EHR.component.css'],
  providers: [EHRService]
})
export class EHRComponent implements OnInit {

  myForm: FormGroup;

  private allAssets;
  private asset;
  private currentId;
  private errorMessage;

  ehrId = new FormControl('', Validators.required);
  age = new FormControl('', Validators.required);
  prescription = new FormControl('', Validators.required);
  diagnosis = new FormControl('', Validators.required);
  notes = new FormControl('', Validators.required);
  otherdata = new FormControl('', Validators.required);

  constructor(public serviceEHR: EHRService, fb: FormBuilder) {
    this.myForm = fb.group({
      ehrId: this.ehrId,
      age: this.age,
      prescription: this.prescription,
      diagnosis: this.diagnosis,
      notes: this.notes,
      otherdata: this.otherdata
    });
  };

  ngOnInit(): void {
    this.loadAll();
  }

  loadAll(): Promise<any> {
    const tempList = [];
    return this.serviceEHR.getAll()
    .toPromise()
    .then((result) => {
      this.errorMessage = null;
      result.forEach(asset => {
        tempList.push(asset);
      });
      this.allAssets = tempList;
    })
    .catch((error) => {
      if (error === 'Server error') {
        this.errorMessage = 'Could not connect to REST server. Please check your configuration details';
      } else if (error === '404 - Not Found') {
        this.errorMessage = '404 - Could not find API route. Please check your available APIs.';
      } else {
        this.errorMessage = error;
      }
    });
  }

	/**
   * Event handler for changing the checked state of a checkbox (handles array enumeration values)
   * @param {String} name - the name of the asset field to update
   * @param {any} value - the enumeration value for which to toggle the checked state
   */
  changeArrayValue(name: string, value: any): void {
    const index = this[name].value.indexOf(value);
    if (index === -1) {
      this[name].value.push(value);
    } else {
      this[name].value.splice(index, 1);
    }
  }

	/**
	 * Checkbox helper, determining whether an enumeration value should be selected or not (for array enumeration values
   * only). This is used for checkboxes in the asset updateDialog.
   * @param {String} name - the name of the asset field to check
   * @param {any} value - the enumeration value to check for
   * @return {Boolean} whether the specified asset field contains the provided value
   */
  hasArrayValue(name: string, value: any): boolean {
    return this[name].value.indexOf(value) !== -1;
  }

  addAsset(form: any): Promise<any> {
    this.asset = {
      $class: 'api.EHR',
      'ehrId': this.ehrId.value,
      'age': this.age.value,
      'prescription': this.prescription.value,
      'diagnosis': this.diagnosis.value,
      'notes': this.notes.value,
      'otherdata': this.otherdata.value
    };

    this.myForm.setValue({
      'ehrId': null,
      'age': null,
      'prescription': null,
      'diagnosis': null,
      'notes': null,
      'otherdata': null
    });

    return this.serviceEHR.addAsset(this.asset)
    .toPromise()
    .then(() => {
      this.errorMessage = null;
      this.myForm.setValue({
        'ehrId': null,
        'age': null,
        'prescription': null,
        'diagnosis': null,
        'notes': null,
        'otherdata': null
      });
      this.loadAll();
    })
    .catch((error) => {
      if (error === 'Server error') {
          this.errorMessage = 'Could not connect to REST server. Please check your configuration details';
      } else {
          this.errorMessage = error;
      }
    });
  }


  updateAsset(form: any): Promise<any> {
    this.asset = {
      $class: 'api.EHR',
      'age': this.age.value,
      'prescription': this.prescription.value,
      'diagnosis': this.diagnosis.value,
      'notes': this.notes.value,
      'otherdata': this.otherdata.value
    };

    return this.serviceEHR.updateAsset(form.get('ehrId').value, this.asset)
    .toPromise()
    .then(() => {
      this.errorMessage = null;
      this.loadAll();
    })
    .catch((error) => {
      if (error === 'Server error') {
        this.errorMessage = 'Could not connect to REST server. Please check your configuration details';
      } else if (error === '404 - Not Found') {
        this.errorMessage = '404 - Could not find API route. Please check your available APIs.';
      } else {
        this.errorMessage = error;
      }
    });
  }


  deleteAsset(): Promise<any> {

    return this.serviceEHR.deleteAsset(this.currentId)
    .toPromise()
    .then(() => {
      this.errorMessage = null;
      this.loadAll();
    })
    .catch((error) => {
      if (error === 'Server error') {
        this.errorMessage = 'Could not connect to REST server. Please check your configuration details';
      } else if (error === '404 - Not Found') {
        this.errorMessage = '404 - Could not find API route. Please check your available APIs.';
      } else {
        this.errorMessage = error;
      }
    });
  }

  setId(id: any): void {
    this.currentId = id;
  }

  getForm(id: any): Promise<any> {

    return this.serviceEHR.getAsset(id)
    .toPromise()
    .then((result) => {
      this.errorMessage = null;
      const formObject = {
        'ehrId': null,
        'age': null,
        'prescription': null,
        'diagnosis': null,
        'notes': null,
        'otherdata': null
      };

      if (result.ehrId) {
        formObject.ehrId = result.ehrId;
      } else {
        formObject.ehrId = null;
      }

      if (result.age) {
        formObject.age = result.age;
      } else {
        formObject.age = null;
      }

      if (result.prescription) {
        formObject.prescription = result.prescription;
      } else {
        formObject.prescription = null;
      }

      if (result.diagnosis) {
        formObject.diagnosis = result.diagnosis;
      } else {
        formObject.diagnosis = null;
      }

      if (result.notes) {
        formObject.notes = result.notes;
      } else {
        formObject.notes = null;
      }

      if (result.otherdata) {
        formObject.otherdata = result.otherdata;
      } else {
        formObject.otherdata = null;
      }

      this.myForm.setValue(formObject);

    })
    .catch((error) => {
      if (error === 'Server error') {
        this.errorMessage = 'Could not connect to REST server. Please check your configuration details';
      } else if (error === '404 - Not Found') {
        this.errorMessage = '404 - Could not find API route. Please check your available APIs.';
      } else {
        this.errorMessage = error;
      }
    });
  }

  resetForm(): void {
    this.myForm.setValue({
      'ehrId': null,
      'age': null,
      'prescription': null,
      'diagnosis': null,
      'notes': null,
      'otherdata': null
      });
  }

}
